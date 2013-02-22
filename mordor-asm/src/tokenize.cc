#include <stdlib.h>

#include <coin/utils/Stream.h>

#include <mdr/def/Type.h>
#include <mdr/api/Type.h>

#include "main.h"

using namespace std;
using namespace coin;


namespace mdr {
namespace assembler {

namespace {

void createStringForToken (vector<char>& buffer, char** strp) {
    size_t nChars = buffer.size ();
    char* str = new char[nChars + 1];
    std::copy (buffer.begin (), buffer.end (), str);
    str[nChars] = '\0';
    *strp = str;
}

bool isNumber (char c, bool beginning) {
    if (beginning) {
        if (c == '-') return true;
    }
    return c >= '0' && c <= '9';
}

bool isLiteralCharacter (char c, bool beginning) {
    if (!beginning) {
        if (isNumber (c, false)) return true;
    }
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c == '.';
}

Token* addToken (vector<Token*>& tokens, int tag) {
    Token* token = new Token ();
    token->tag = tag;
    tokens.push_back (token);
    return token;
}

size_t skipToNextLine (char* data, size_t length, size_t index) {
    for (; index < length; ++index) {
        if (data[index] == '\n') break;
    }

    return index + 1;
}

size_t addNumberToken (vector<Token*>& tokens, char* data, size_t length, size_t index) {
    // TODO(Marco): Add hexadecimal base.

    vector<char> buffer;

    mdrType type = MDR_TYPE_I32;
    bool has_dot = false;

    /* Find the number and determine the type. */
    size_t start = index;
    for (; index < length; ++index) {
        char c = data[index];
        
        if (c == '.' && !has_dot) {
            has_dot = true;
            buffer.push_back (c);
        }else if (isNumber (c, index == start)) {
            buffer.push_back (c);
        }else if (c == 'f') {
            type = MDR_TYPE_F32;
        }else if (c == 'u') {
            type = MDR_TYPE_U32;
        }else if (c == 'l') {
            mdrTypeAddSize (type); // TODO(Marco): Add check to disallow size adding to already 64bit types.
        }else {
            break;
        }
    }

    char* str;
    createStringForToken (buffer, &str);

    /* Parse values according to the type. */
    // TODO(Marco): The strtoX or atoX have to do it for now, until I write a faster specialized library for that. Also note that some functions depend on Visual Studio.

    Token* token = addToken (tokens, TOKEN_CONSTANT);
    token->constant.type = type;

    switch (type) {
        case MDR_TYPE_I32:
            token->constant.value._s32 = (mdr_s32) atoi (str);
            break;
        case MDR_TYPE_I64:
            token->constant.value._s64 = (mdr_s64) _strtoi64 (str, NULL, 10);
            break;
        case MDR_TYPE_U32:
            token->constant.value._u32 = (mdr_u32) atoi (str);
            break;
        case MDR_TYPE_U64:
            token->constant.value._u64 = (mdr_u64) _strtoui64 (str, NULL, 10);
            break;
        case MDR_TYPE_F32:
            token->constant.value._f32 = (mdr_f32) strtod (str, NULL);
            break;
        case MDR_TYPE_F64:
            token->constant.value._f64 = (mdr_f64) strtod (str, NULL);
            break;
        default:
            break;
    }

    delete[] str;
    return index + 1;
}

size_t addStringToken (vector<Token*>& tokens, char* data, size_t length, size_t index) {
    vector<char> buffer;
    char last_char = '\0';
    ++index; /* Skip string delimiter ('"'). */
    for (; index < length; ++index) {
        char c = data[index];

        if (c == '"' && last_char != '\\') {
            break;
        }

        buffer.push_back (c);
        last_char = c;
    }

    Token* token = addToken (tokens, TOKEN_STRING);
    createStringForToken (buffer, &token->string);

    return index + 1;
}

size_t addLiteralToken (vector<Token*>& tokens, char* data, size_t length, size_t index) {
    vector<char> buffer;
    for (size_t start = index; index < length; ++index) {
        char c = data[index];
        
        if (isLiteralCharacter (c, index == start)) {
            buffer.push_back (c);
        }else {
            break;
        }
    }

    Token* token = addToken (tokens, TOKEN_LITERAL);
    createStringForToken (buffer, &token->string);

    return index;
}

}


void TokenizeFile (const char* file_path, vector<Token*>& tokens) {
    FileStream stream (file_path, StreamMode::read);

    const Size file_size = stream.Size ();
    char* data = new char[file_size];
    stream.Read ((u8*) data, file_size);

    for (size_t i = 0; i < file_size;) {
        char c = data[i];

        if (c == ' ' || c == '\t' || c == '\r' || c == '(' || c == ')' || c == ',') {
            ++i;
            continue;
        }

        if (c == '\n' || c == ':') {
            bool add_token = true;
            if (c == '\n') { /* Skip adding a character token if last token was '\n'. */
                if (tokens.size () > 0) {
                    Token* last_token = tokens[tokens.size () - 1];
                    if (last_token->tag == TOKEN_CHARACTER && last_token->character == '\n') {
                        add_token = false;
                    }
                }else { /* Don't add as first token either. */
                    add_token = false;
                }
            }

            if (add_token) {
                addToken (tokens, TOKEN_CHARACTER)->character = c;
            }

            ++i;
            continue;
        }

        if (c == '#') {
            i = skipToNextLine (data, file_size, i);
            continue;
        }

        if (isNumber (c, true)) {
            i = addNumberToken (tokens, data, file_size, i);
            continue;
        }

        if (c == '"') {
            i = addStringToken (tokens, data, file_size, i);
            continue;
        }

        if (isLiteralCharacter (c, true)) {
            i = addLiteralToken (tokens, data, file_size, i);
            continue; 
        }

        ++i;
    }

    /* Add '\n' as last token if not already an '\n' is the last token. */
    Token* last_token = tokens[tokens.size () - 1];
    if (last_token->tag != TOKEN_CHARACTER || last_token->character != '\n') {
        addToken (tokens, TOKEN_CHARACTER)->character = '\n';
    }

}

}
}