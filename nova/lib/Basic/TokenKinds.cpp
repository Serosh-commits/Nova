#include "nova/Basic/TokenKinds.h"

using namespace nova;

static const char * const TokNames[] = {
#define TOK(X) #X,
#define KEYWORD(X, Y) #X,
#include "nova/Basic/TokenKinds.def"
  nullptr
};

const char *tok::getTokenName(TokenKind Kind) noexcept {
  if (Kind < tok::NUM_TOKENS)
    return TokNames[Kind];
  return "unknown";
}

const char *tok::getPunctuatorSpelling(TokenKind Kind) noexcept {
  switch (Kind) {
#define PUNCTUATOR(X, Y) case X: return Y;
#include "nova/Basic/TokenKinds.def"
  default: break;
  }
  return nullptr;
}

const char *tok::getKeywordSpelling(TokenKind Kind) noexcept {
  switch (Kind) {
#define KEYWORD(X, Y) case kw_ ## X: return #X;
#include "nova/Basic/TokenKinds.def"
  default: break;
  }
  return nullptr;
}
