// Generated by Cap'n Proto compiler, DO NOT EDIT
// source: Command.capnp

#ifndef CAPNP_INCLUDED_abf8f53c554fa923_
#define CAPNP_INCLUDED_abf8f53c554fa923_

#include <capnp/generated-header-support.h>

#if CAPNP_VERSION != 6001
#error "Version mismatch between generated code and library headers.  You must use the same version of the Cap'n Proto compiler and library."
#endif

#include "Common.capnp.h"

namespace capnp {
namespace schemas {

CAPNP_DECLARE_SCHEMA(ac5e264a1547dce3);
CAPNP_DECLARE_SCHEMA(c0b4b2c3dfb664f2);
CAPNP_DECLARE_SCHEMA(a027b1c0251d209e);
CAPNP_DECLARE_SCHEMA(9d69a170a5fdbc5a);

}  // namespace schemas
}  // namespace capnp

namespace protocol {

struct Move {
  Move() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(ac5e264a1547dce3, 1, 0)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Command {
  Command() = delete;

  class Reader;
  class Builder;
  class Pipeline;
  struct Commands;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(c0b4b2c3dfb664f2, 1, 2)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Command::Commands {
  Commands() = delete;

  class Reader;
  class Builder;
  class Pipeline;
  enum Which: uint16_t {
    MOVES,
    LOGIN,
  };
  struct Login;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(a027b1c0251d209e, 1, 2)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

struct Command::Commands::Login {
  Login() = delete;

  class Reader;
  class Builder;
  class Pipeline;

  struct _capnpPrivate {
    CAPNP_DECLARE_STRUCT_HEADER(9d69a170a5fdbc5a, 1, 2)
    #if !CAPNP_LITE
    static constexpr ::capnp::_::RawBrandedSchema const* brand() { return &schema->defaultBrand; }
    #endif  // !CAPNP_LITE
  };
};

// =======================================================================================

class Move::Reader {
public:
  typedef Move Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline  ::int32_t getUnit() const;

  inline  ::protocol::Direction getDirection() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Move::Builder {
public:
  typedef Move Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline  ::int32_t getUnit();
  inline void setUnit( ::int32_t value);

  inline  ::protocol::Direction getDirection();
  inline void setDirection( ::protocol::Direction value);

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Move::Pipeline {
public:
  typedef Move Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Command::Reader {
public:
  typedef Command Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline typename Commands::Reader getCommands() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Command::Builder {
public:
  typedef Command Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline typename Commands::Builder getCommands();
  inline typename Commands::Builder initCommands();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Command::Pipeline {
public:
  typedef Command Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

  inline typename Commands::Pipeline getCommands();
private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Command::Commands::Reader {
public:
  typedef Commands Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline Which which() const;
  inline bool isMoves() const;
  inline bool hasMoves() const;
  inline  ::capnp::List< ::protocol::Move>::Reader getMoves() const;

  inline bool isLogin() const;
  inline typename Login::Reader getLogin() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Command::Commands::Builder {
public:
  typedef Commands Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline Which which();
  inline bool isMoves();
  inline bool hasMoves();
  inline  ::capnp::List< ::protocol::Move>::Builder getMoves();
  inline void setMoves( ::capnp::List< ::protocol::Move>::Reader value);
  inline  ::capnp::List< ::protocol::Move>::Builder initMoves(unsigned int size);
  inline void adoptMoves(::capnp::Orphan< ::capnp::List< ::protocol::Move>>&& value);
  inline ::capnp::Orphan< ::capnp::List< ::protocol::Move>> disownMoves();

  inline bool isLogin();
  inline typename Login::Builder getLogin();
  inline typename Login::Builder initLogin();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Command::Commands::Pipeline {
public:
  typedef Commands Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

class Command::Commands::Login::Reader {
public:
  typedef Login Reads;

  Reader() = default;
  inline explicit Reader(::capnp::_::StructReader base): _reader(base) {}

  inline ::capnp::MessageSize totalSize() const {
    return _reader.totalSize().asPublic();
  }

#if !CAPNP_LITE
  inline ::kj::StringTree toString() const {
    return ::capnp::_::structString(_reader, *_capnpPrivate::brand());
  }
#endif  // !CAPNP_LITE

  inline bool hasTeam() const;
  inline  ::capnp::Text::Reader getTeam() const;

  inline bool hasHash() const;
  inline  ::capnp::Text::Reader getHash() const;

private:
  ::capnp::_::StructReader _reader;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::List;
  friend class ::capnp::MessageBuilder;
  friend class ::capnp::Orphanage;
};

class Command::Commands::Login::Builder {
public:
  typedef Login Builds;

  Builder() = delete;  // Deleted to discourage incorrect usage.
                       // You can explicitly initialize to nullptr instead.
  inline Builder(decltype(nullptr)) {}
  inline explicit Builder(::capnp::_::StructBuilder base): _builder(base) {}
  inline operator Reader() const { return Reader(_builder.asReader()); }
  inline Reader asReader() const { return *this; }

  inline ::capnp::MessageSize totalSize() const { return asReader().totalSize(); }
#if !CAPNP_LITE
  inline ::kj::StringTree toString() const { return asReader().toString(); }
#endif  // !CAPNP_LITE

  inline bool hasTeam();
  inline  ::capnp::Text::Builder getTeam();
  inline void setTeam( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initTeam(unsigned int size);
  inline void adoptTeam(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownTeam();

  inline bool hasHash();
  inline  ::capnp::Text::Builder getHash();
  inline void setHash( ::capnp::Text::Reader value);
  inline  ::capnp::Text::Builder initHash(unsigned int size);
  inline void adoptHash(::capnp::Orphan< ::capnp::Text>&& value);
  inline ::capnp::Orphan< ::capnp::Text> disownHash();

private:
  ::capnp::_::StructBuilder _builder;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
  friend class ::capnp::Orphanage;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::_::PointerHelpers;
};

#if !CAPNP_LITE
class Command::Commands::Login::Pipeline {
public:
  typedef Login Pipelines;

  inline Pipeline(decltype(nullptr)): _typeless(nullptr) {}
  inline explicit Pipeline(::capnp::AnyPointer::Pipeline&& typeless)
      : _typeless(kj::mv(typeless)) {}

private:
  ::capnp::AnyPointer::Pipeline _typeless;
  friend class ::capnp::PipelineHook;
  template <typename, ::capnp::Kind>
  friend struct ::capnp::ToDynamic_;
};
#endif  // !CAPNP_LITE

// =======================================================================================

inline  ::int32_t Move::Reader::getUnit() const {
  return _reader.getDataField< ::int32_t>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline  ::int32_t Move::Builder::getUnit() {
  return _builder.getDataField< ::int32_t>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline void Move::Builder::setUnit( ::int32_t value) {
  _builder.setDataField< ::int32_t>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, value);
}

inline  ::protocol::Direction Move::Reader::getDirection() const {
  return _reader.getDataField< ::protocol::Direction>(
      ::capnp::bounded<2>() * ::capnp::ELEMENTS);
}

inline  ::protocol::Direction Move::Builder::getDirection() {
  return _builder.getDataField< ::protocol::Direction>(
      ::capnp::bounded<2>() * ::capnp::ELEMENTS);
}
inline void Move::Builder::setDirection( ::protocol::Direction value) {
  _builder.setDataField< ::protocol::Direction>(
      ::capnp::bounded<2>() * ::capnp::ELEMENTS, value);
}

inline typename Command::Commands::Reader Command::Reader::getCommands() const {
  return typename Command::Commands::Reader(_reader);
}
inline typename Command::Commands::Builder Command::Builder::getCommands() {
  return typename Command::Commands::Builder(_builder);
}
#if !CAPNP_LITE
inline typename Command::Commands::Pipeline Command::Pipeline::getCommands() {
  return typename Command::Commands::Pipeline(_typeless.noop());
}
#endif  // !CAPNP_LITE
inline typename Command::Commands::Builder Command::Builder::initCommands() {
  _builder.setDataField< ::uint16_t>(::capnp::bounded<0>() * ::capnp::ELEMENTS, 0);
  _builder.getPointerField(::capnp::bounded<0>() * ::capnp::POINTERS).clear();
  _builder.getPointerField(::capnp::bounded<1>() * ::capnp::POINTERS).clear();
  return typename Command::Commands::Builder(_builder);
}
inline  ::protocol::Command::Commands::Which Command::Commands::Reader::which() const {
  return _reader.getDataField<Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}
inline  ::protocol::Command::Commands::Which Command::Commands::Builder::which() {
  return _builder.getDataField<Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS);
}

inline bool Command::Commands::Reader::isMoves() const {
  return which() == Command::Commands::MOVES;
}
inline bool Command::Commands::Builder::isMoves() {
  return which() == Command::Commands::MOVES;
}
inline bool Command::Commands::Reader::hasMoves() const {
  if (which() != Command::Commands::MOVES) return false;
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool Command::Commands::Builder::hasMoves() {
  if (which() != Command::Commands::MOVES) return false;
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::List< ::protocol::Move>::Reader Command::Commands::Reader::getMoves() const {
  KJ_IREQUIRE((which() == Command::Commands::MOVES),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::capnp::List< ::protocol::Move>>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::capnp::List< ::protocol::Move>::Builder Command::Commands::Builder::getMoves() {
  KJ_IREQUIRE((which() == Command::Commands::MOVES),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::capnp::List< ::protocol::Move>>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void Command::Commands::Builder::setMoves( ::capnp::List< ::protocol::Move>::Reader value) {
  _builder.setDataField<Command::Commands::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, Command::Commands::MOVES);
  ::capnp::_::PointerHelpers< ::capnp::List< ::protocol::Move>>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::capnp::List< ::protocol::Move>::Builder Command::Commands::Builder::initMoves(unsigned int size) {
  _builder.setDataField<Command::Commands::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, Command::Commands::MOVES);
  return ::capnp::_::PointerHelpers< ::capnp::List< ::protocol::Move>>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), size);
}
inline void Command::Commands::Builder::adoptMoves(
    ::capnp::Orphan< ::capnp::List< ::protocol::Move>>&& value) {
  _builder.setDataField<Command::Commands::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, Command::Commands::MOVES);
  ::capnp::_::PointerHelpers< ::capnp::List< ::protocol::Move>>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::List< ::protocol::Move>> Command::Commands::Builder::disownMoves() {
  KJ_IREQUIRE((which() == Command::Commands::MOVES),
              "Must check which() before get()ing a union member.");
  return ::capnp::_::PointerHelpers< ::capnp::List< ::protocol::Move>>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline bool Command::Commands::Reader::isLogin() const {
  return which() == Command::Commands::LOGIN;
}
inline bool Command::Commands::Builder::isLogin() {
  return which() == Command::Commands::LOGIN;
}
inline typename Command::Commands::Login::Reader Command::Commands::Reader::getLogin() const {
  KJ_IREQUIRE((which() == Command::Commands::LOGIN),
              "Must check which() before get()ing a union member.");
  return typename Command::Commands::Login::Reader(_reader);
}
inline typename Command::Commands::Login::Builder Command::Commands::Builder::getLogin() {
  KJ_IREQUIRE((which() == Command::Commands::LOGIN),
              "Must check which() before get()ing a union member.");
  return typename Command::Commands::Login::Builder(_builder);
}
inline typename Command::Commands::Login::Builder Command::Commands::Builder::initLogin() {
  _builder.setDataField<Command::Commands::Which>(
      ::capnp::bounded<0>() * ::capnp::ELEMENTS, Command::Commands::LOGIN);
  _builder.getPointerField(::capnp::bounded<0>() * ::capnp::POINTERS).clear();
  _builder.getPointerField(::capnp::bounded<1>() * ::capnp::POINTERS).clear();
  return typename Command::Commands::Login::Builder(_builder);
}
inline bool Command::Commands::Login::Reader::hasTeam() const {
  return !_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline bool Command::Commands::Login::Builder::hasTeam() {
  return !_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader Command::Commands::Login::Reader::getTeam() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_reader.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder Command::Commands::Login::Builder::getTeam() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}
inline void Command::Commands::Login::Builder::setTeam( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder Command::Commands::Login::Builder::initTeam(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), size);
}
inline void Command::Commands::Login::Builder::adoptTeam(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> Command::Commands::Login::Builder::disownTeam() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(_builder.getPointerField(
      ::capnp::bounded<0>() * ::capnp::POINTERS));
}

inline bool Command::Commands::Login::Reader::hasHash() const {
  return !_reader.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS).isNull();
}
inline bool Command::Commands::Login::Builder::hasHash() {
  return !_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS).isNull();
}
inline  ::capnp::Text::Reader Command::Commands::Login::Reader::getHash() const {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_reader.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}
inline  ::capnp::Text::Builder Command::Commands::Login::Builder::getHash() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::get(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}
inline void Command::Commands::Login::Builder::setHash( ::capnp::Text::Reader value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::set(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), value);
}
inline  ::capnp::Text::Builder Command::Commands::Login::Builder::initHash(unsigned int size) {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::init(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), size);
}
inline void Command::Commands::Login::Builder::adoptHash(
    ::capnp::Orphan< ::capnp::Text>&& value) {
  ::capnp::_::PointerHelpers< ::capnp::Text>::adopt(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS), kj::mv(value));
}
inline ::capnp::Orphan< ::capnp::Text> Command::Commands::Login::Builder::disownHash() {
  return ::capnp::_::PointerHelpers< ::capnp::Text>::disown(_builder.getPointerField(
      ::capnp::bounded<1>() * ::capnp::POINTERS));
}

}  // namespace

#endif  // CAPNP_INCLUDED_abf8f53c554fa923_