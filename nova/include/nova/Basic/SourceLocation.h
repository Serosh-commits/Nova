#pragma once

#include <cassert>
#include <cstdint>
#include <utility>

namespace nova {

class SourceManager;

class FileID {
  int ID = 0;

public:
  constexpr FileID() noexcept = default;

  [[nodiscard]] constexpr bool isValid() const noexcept { return ID != 0; }
  [[nodiscard]] constexpr bool isInvalid() const noexcept { return ID == 0; }

  constexpr bool operator==(const FileID &RHS) const noexcept {
    return ID == RHS.ID;
  }
  constexpr bool operator!=(const FileID &RHS) const noexcept {
    return !(*this == RHS);
  }
  constexpr bool operator<(const FileID &RHS) const noexcept {
    return ID < RHS.ID;
  }
  constexpr bool operator<=(const FileID &RHS) const noexcept {
    return ID <= RHS.ID;
  }
  constexpr bool operator>(const FileID &RHS) const noexcept {
    return RHS < *this;
  }
  constexpr bool operator>=(const FileID &RHS) const noexcept {
    return RHS <= *this;
  }

  [[nodiscard]] static constexpr FileID getSentinel() noexcept {
    return get(-1);
  }
  [[nodiscard]] unsigned getHashValue() const noexcept {
    return static_cast<unsigned>(ID);
  }

private:
  friend class SourceManager;

  [[nodiscard]] static constexpr FileID get(int V) noexcept {
    FileID F;
    F.ID = V;
    return F;
  }

  [[nodiscard]] constexpr int getOpaqueValue() const noexcept { return ID; }
};

class SourceLocation {
public:
  using UIntTy = uint32_t;
  using IntTy = int32_t;

private:
  UIntTy ID = 0;
  enum : UIntTy { MacroIDBit = 1ULL << (8 * sizeof(UIntTy) - 1) };

public:
  constexpr SourceLocation() noexcept = default;

  [[nodiscard]] constexpr bool isFileID() const noexcept {
    return (ID & MacroIDBit) == 0;
  }
  [[nodiscard]] constexpr bool isMacroID() const noexcept {
    return (ID & MacroIDBit) != 0;
  }

  [[nodiscard]] constexpr bool isValid() const noexcept { return ID != 0; }
  [[nodiscard]] constexpr bool isInvalid() const noexcept { return ID == 0; }

  [[nodiscard]] constexpr UIntTy getRawEncoding() const noexcept { return ID; }

  [[nodiscard]] static constexpr SourceLocation
  getFromRawEncoding(UIntTy Encoding) noexcept {
    SourceLocation X;
    X.ID = Encoding;
    return X;
  }

  [[nodiscard]] constexpr SourceLocation
  getLocWithOffset(IntTy Offset) const noexcept {
    assert(((getOffset() + Offset) & MacroIDBit) == 0 && "offset overflow");
    SourceLocation L;
    L.ID = ID + Offset;
    return L;
  }

  constexpr bool operator==(const SourceLocation &RHS) const noexcept {
    return ID == RHS.ID;
  }
  constexpr bool operator!=(const SourceLocation &RHS) const noexcept {
    return !(*this == RHS);
  }
  constexpr bool operator<(const SourceLocation &RHS) const noexcept {
    return ID < RHS.ID;
  }
  constexpr bool operator<=(const SourceLocation &RHS) const noexcept {
    return ID <= RHS.ID;
  }
  constexpr bool operator>(const SourceLocation &RHS) const noexcept {
    return RHS < *this;
  }
  constexpr bool operator>=(const SourceLocation &RHS) const noexcept {
    return RHS <= *this;
  }

  [[nodiscard]] static bool isPairOfFileLocations(SourceLocation Start,
                                                  SourceLocation End) noexcept {
    return Start.isValid() && Start.isFileID() && End.isValid() &&
           End.isFileID();
  }

  [[nodiscard]] unsigned getHashValue() const noexcept { return ID; }

private:
  friend class SourceManager;

  [[nodiscard]] constexpr UIntTy getOffset() const noexcept {
    return ID & ~MacroIDBit;
  }

  [[nodiscard]] static constexpr SourceLocation getFileLoc(UIntTy ID) noexcept {
    assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
    SourceLocation L;
    L.ID = ID;
    return L;
  }

  [[nodiscard]] static constexpr SourceLocation
  getMacroLoc(UIntTy ID) noexcept {
    assert((ID & MacroIDBit) == 0 && "Ran out of source locations!");
    SourceLocation L;
    L.ID = MacroIDBit | ID;
    return L;
  }
};

class SourceRange {
  SourceLocation B;
  SourceLocation E;

public:
  constexpr SourceRange() noexcept = default;
  constexpr SourceRange(SourceLocation loc) noexcept : B(loc), E(loc) {}
  constexpr SourceRange(SourceLocation begin, SourceLocation end) noexcept
      : B(begin), E(end) {}

  [[nodiscard]] constexpr SourceLocation getBegin() const noexcept { return B; }
  [[nodiscard]] constexpr SourceLocation getEnd() const noexcept { return E; }

  constexpr void setBegin(SourceLocation b) noexcept { B = b; }
  constexpr void setEnd(SourceLocation e) noexcept { E = e; }

  [[nodiscard]] constexpr bool isValid() const noexcept {
    return B.isValid() && E.isValid();
  }
  [[nodiscard]] constexpr bool isInvalid() const noexcept { return !isValid(); }

  constexpr bool operator==(const SourceRange &X) const noexcept {
    return B == X.B && E == X.E;
  }
  constexpr bool operator!=(const SourceRange &X) const noexcept {
    return !(*this == X);
  }

  [[nodiscard]] bool fullyContains(const SourceRange &other) const noexcept {
    return B <= other.B && E >= other.E;
  }
};

class CharSourceRange {
  SourceRange Range;
  bool IsTokenRange = false;

public:
  constexpr CharSourceRange() noexcept = default;
  constexpr CharSourceRange(SourceRange R, bool ITR) noexcept
      : Range(R), IsTokenRange(ITR) {}

  [[nodiscard]] static constexpr CharSourceRange
  getTokenRange(SourceRange R) noexcept {
    return CharSourceRange(R, true);
  }
  [[nodiscard]] static constexpr CharSourceRange
  getCharRange(SourceRange R) noexcept {
    return CharSourceRange(R, false);
  }
  [[nodiscard]] static constexpr CharSourceRange
  getTokenRange(SourceLocation B, SourceLocation E) noexcept {
    return getTokenRange(SourceRange(B, E));
  }
  [[nodiscard]] static constexpr CharSourceRange
  getCharRange(SourceLocation B, SourceLocation E) noexcept {
    return getCharRange(SourceRange(B, E));
  }

  [[nodiscard]] constexpr bool isTokenRange() const noexcept {
    return IsTokenRange;
  }
  [[nodiscard]] constexpr bool isCharRange() const noexcept {
    return !IsTokenRange;
  }

  [[nodiscard]] constexpr SourceLocation getBegin() const noexcept {
    return Range.getBegin();
  }
  [[nodiscard]] constexpr SourceLocation getEnd() const noexcept {
    return Range.getEnd();
  }
  [[nodiscard]] constexpr SourceRange getAsRange() const noexcept {
    return Range;
  }

  constexpr void setBegin(SourceLocation b) noexcept { Range.setBegin(b); }
  constexpr void setEnd(SourceLocation e) noexcept { Range.setEnd(e); }

  [[nodiscard]] constexpr bool isValid() const noexcept {
    return Range.isValid();
  }
  [[nodiscard]] constexpr bool isInvalid() const noexcept {
    return Range.isInvalid();
  }
};

class PresumedLoc {
  const char *Filename = nullptr;
  FileID ID;
  unsigned Line = 0;
  unsigned Col = 0;
  SourceLocation IncludeLoc;

public:
  PresumedLoc() noexcept = default;
  PresumedLoc(const char *FN, FileID FID, unsigned L, unsigned C,
              SourceLocation IL) noexcept
      : Filename(FN), ID(FID), Line(L), Col(C), IncludeLoc(IL) {}

  [[nodiscard]] bool isValid() const noexcept { return Filename != nullptr; }
  [[nodiscard]] bool isInvalid() const noexcept { return !isValid(); }

  [[nodiscard]] const char *getFilename() const noexcept { return Filename; }
  [[nodiscard]] FileID getFileID() const noexcept { return ID; }
  [[nodiscard]] unsigned getLine() const noexcept { return Line; }
  [[nodiscard]] unsigned getColumn() const noexcept { return Col; }
  [[nodiscard]] SourceLocation getIncludeLoc() const noexcept {
    return IncludeLoc;
  }
};

class FullSourceLoc : public SourceLocation {
  const SourceManager *SrcMgr = nullptr;

public:
  FullSourceLoc() noexcept = default;
  FullSourceLoc(SourceLocation Loc, const SourceManager &Mgr) noexcept
      : SourceLocation(Loc), SrcMgr(&Mgr) {}

  [[nodiscard]] bool hasManager() const noexcept { return SrcMgr != nullptr; }
  [[nodiscard]] const SourceManager &getManager() const noexcept {
    assert(SrcMgr && "SourceManager is null");
    return *SrcMgr;
  }

  [[nodiscard]] FileID getFileID() const noexcept;
  [[nodiscard]] unsigned getSpellingLineNumber() const noexcept;
  [[nodiscard]] unsigned getSpellingColumnNumber() const noexcept;
  [[nodiscard]] const char *getCharacterData() const noexcept;
  [[nodiscard]] PresumedLoc getPresumedLoc() const noexcept;
  [[nodiscard]] bool
  isBeforeInTranslationUnitThan(SourceLocation Loc) const noexcept;

  friend bool operator==(const FullSourceLoc &LHS,
                         const FullSourceLoc &RHS) noexcept {
    return LHS.getRawEncoding() == RHS.getRawEncoding() &&
           LHS.SrcMgr == RHS.SrcMgr;
  }
  friend bool operator!=(const FullSourceLoc &LHS,
                         const FullSourceLoc &RHS) noexcept {
    return !(LHS == RHS);
  }
};

} // namespace nova