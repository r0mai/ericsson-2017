from __future__ import print_function
import capnp
import requests
import socket
import struct


class Proto:
    request = capnp.load('protocol/Request.capnp').Request
    response = capnp.load('protocol/Response.capnp').Response
    bugfix = capnp.load('protocol/Bugfix.capnp').Bugfix

    def __init__(self):
        self.sock = None
        self.buffer = ''
        self.end = False
        self.count = 0

    def login(self):
        ipaddr = socket.gethostbyname('ecovpn.dyndns.org')
        port = 11223
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((ipaddr, port))

        user = 'prezident_evil'
        passwd = 'stzq8jm94kf9iyw7353j9semae2sjorjvthakhzw'
        msg = Proto.request.new_message()
        msg.login.team = user
        msg.login.hash = passwd
        self.sock.send(msg.to_bytes())

    def read(self):
        if not self.end:
            res = self._read_response()
            print('> {}'.format(res))

    def close(self):
        if self.sock:
            self.sock.close()
            self.sock = None

    def solve(self, n, msg='Fixed'):
        res = self._read_response()
        print('> {}'.format(res))
        if res.which() == 'end' and res.end is True:
            self.end = True
            print('Success!')
            return False

        self._send_bugfix(n, msg)
        return True

    def _send_bugfix(self, n, message):
        msg = Proto.request.new_message()
        msg.bugfix = Proto.bugfix.new_message()
        msg.bugfix.bugs = n
        msg.bugfix.message = message
        print('< fixing #{}'.format(n))
        self.sock.send(msg.to_bytes())

    def _read_response(self):
        data = self.buffer
        s = self.sock
        n = 0
        ssize = []
        total_size = 0
        buf_size = 4096

        while True:
            data += s.recv(buf_size)
            if data is None:
                raise IOError('Connection was closed')
            if n == 0 and len(data) < 4:
                continue
            if n == 0:
                n = struct.unpack('<I', data[:4])[0] + 1
            if len(ssize) == 0 and len(data) < 4 + 4 * n:
                continue
            if len(ssize) == 0:
                for i in range(n):
                    ssize.append(struct.unpack('<I', data[4+i*4:4+(i+1)*4])[0])
                padding = 4 if n % 2 == 0 else 0
                total_size = 4 + n * 4 + padding + sum(ssize) * 8
            if len(data) >= total_size:
                break

        self.buffer = data[total_size:]
        msg = Proto.response.from_bytes(data[:total_size])
        self.count += 1
        # with open('response-{:02}.msg'.format(self.count), 'wb') as f:
        #     f.write(data[:total_size])
        return msg


if __name__ == '__main__':
    p = Proto()
    p.login()
    p.solve(9)
    p.solve(8)
    p.solve(19)
    p.solve(18)
    p.solve(17)
    p.solve(16)
    p.solve(2)
    p.solve(2)
    p.solve(2)
    p.solve(1)
    p.solve(0)
    p.solve(255)
    p.solve(12, msg='I solved a huge amount of bug. I am proud of myself.')
    p.read()
    p.close()
