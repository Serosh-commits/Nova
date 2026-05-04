#pragma once

#include <cstdint>

namespace nova {
namespace tok {

enum TokenKind : unsigned short {
#define TOK(X) X,
#include "nova/Basic/TokenKinds.def"
  NUM_TOKENS
};

const char *getTokenName(TokenKind Kind) noexcept;

const char *getPunctuatorSpelling(TokenKind Kind) noexcept;

const char *getKeywordSpelling(TokenKind Kind) noexcept;

} // namespace tok
} // namespace nova
