#include "parser.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#define LANGUAGE_VERSION 14
#define STATE_COUNT 40
#define LARGE_STATE_COUNT 2
#define SYMBOL_COUNT 37
#define ALIAS_COUNT 0
#define TOKEN_COUNT 24
#define EXTERNAL_TOKEN_COUNT 0
#define FIELD_COUNT 0
#define MAX_ALIAS_SEQUENCE_LENGTH 5
#define PRODUCTION_ID_COUNT 1

enum ts_symbol_identifiers {
  aux_sym_number_value_token1 = 1,
  aux_sym_number_token1 = 2,
  anon_sym_DOT = 3,
  sym_string_value = 4,
  anon_sym_DQUOTE = 5,
  sym_id_name = 6,
  anon_sym_LBRACK = 7,
  anon_sym_RBRACK = 8,
  anon_sym_LBRACE = 9,
  anon_sym_COMMA = 10,
  anon_sym_RBRACE = 11,
  anon_sym_EQ_EQ = 12,
  anon_sym_BANG_EQ = 13,
  anon_sym_IN = 14,
  anon_sym_NOTIN = 15,
  anon_sym_GT = 16,
  anon_sym_GT_EQ = 17,
  anon_sym_LT = 18,
  anon_sym_LT_EQ = 19,
  anon_sym_OR = 20,
  anon_sym_AND = 21,
  anon_sym_LPAREN = 22,
  anon_sym_RPAREN = 23,
  sym_source_file = 24,
  sym_number_value = 25,
  sym_number = 26,
  sym_string = 27,
  sym_id_index = 28,
  sym_identifier = 29,
  sym_array = 30,
  sym_compare_op = 31,
  sym_condition = 32,
  sym_logic_op = 33,
  sym_condition_group = 34,
  sym_expression = 35,
  aux_sym_array_repeat1 = 36,
};

static const char * const ts_symbol_names[] = {
  [ts_builtin_sym_end] = "end",
  [aux_sym_number_value_token1] = "number_value_token1",
  [aux_sym_number_token1] = "number_token1",
  [anon_sym_DOT] = ".",
  [sym_string_value] = "string_value",
  [anon_sym_DQUOTE] = "\"",
  [sym_id_name] = "id_name",
  [anon_sym_LBRACK] = "[",
  [anon_sym_RBRACK] = "]",
  [anon_sym_LBRACE] = "{",
  [anon_sym_COMMA] = ",",
  [anon_sym_RBRACE] = "}",
  [anon_sym_EQ_EQ] = "==",
  [anon_sym_BANG_EQ] = "!=",
  [anon_sym_IN] = "IN",
  [anon_sym_NOTIN] = "NOT IN",
  [anon_sym_GT] = ">",
  [anon_sym_GT_EQ] = ">=",
  [anon_sym_LT] = "<",
  [anon_sym_LT_EQ] = "<=",
  [anon_sym_OR] = "OR",
  [anon_sym_AND] = "AND",
  [anon_sym_LPAREN] = "(",
  [anon_sym_RPAREN] = ")",
  [sym_source_file] = "source_file",
  [sym_number_value] = "number_value",
  [sym_number] = "number",
  [sym_string] = "string",
  [sym_id_index] = "id_index",
  [sym_identifier] = "identifier",
  [sym_array] = "array",
  [sym_compare_op] = "compare_op",
  [sym_condition] = "condition",
  [sym_logic_op] = "logic_op",
  [sym_condition_group] = "condition_group",
  [sym_expression] = "expression",
  [aux_sym_array_repeat1] = "array_repeat1",
};

static const TSSymbol ts_symbol_map[] = {
  [ts_builtin_sym_end] = ts_builtin_sym_end,
  [aux_sym_number_value_token1] = aux_sym_number_value_token1,
  [aux_sym_number_token1] = aux_sym_number_token1,
  [anon_sym_DOT] = anon_sym_DOT,
  [sym_string_value] = sym_string_value,
  [anon_sym_DQUOTE] = anon_sym_DQUOTE,
  [sym_id_name] = sym_id_name,
  [anon_sym_LBRACK] = anon_sym_LBRACK,
  [anon_sym_RBRACK] = anon_sym_RBRACK,
  [anon_sym_LBRACE] = anon_sym_LBRACE,
  [anon_sym_COMMA] = anon_sym_COMMA,
  [anon_sym_RBRACE] = anon_sym_RBRACE,
  [anon_sym_EQ_EQ] = anon_sym_EQ_EQ,
  [anon_sym_BANG_EQ] = anon_sym_BANG_EQ,
  [anon_sym_IN] = anon_sym_IN,
  [anon_sym_NOTIN] = anon_sym_NOTIN,
  [anon_sym_GT] = anon_sym_GT,
  [anon_sym_GT_EQ] = anon_sym_GT_EQ,
  [anon_sym_LT] = anon_sym_LT,
  [anon_sym_LT_EQ] = anon_sym_LT_EQ,
  [anon_sym_OR] = anon_sym_OR,
  [anon_sym_AND] = anon_sym_AND,
  [anon_sym_LPAREN] = anon_sym_LPAREN,
  [anon_sym_RPAREN] = anon_sym_RPAREN,
  [sym_source_file] = sym_source_file,
  [sym_number_value] = sym_number_value,
  [sym_number] = sym_number,
  [sym_string] = sym_string,
  [sym_id_index] = sym_id_index,
  [sym_identifier] = sym_identifier,
  [sym_array] = sym_array,
  [sym_compare_op] = sym_compare_op,
  [sym_condition] = sym_condition,
  [sym_logic_op] = sym_logic_op,
  [sym_condition_group] = sym_condition_group,
  [sym_expression] = sym_expression,
  [aux_sym_array_repeat1] = aux_sym_array_repeat1,
};

static const TSSymbolMetadata ts_symbol_metadata[] = {
  [ts_builtin_sym_end] = {
    .visible = false,
    .named = true,
  },
  [aux_sym_number_value_token1] = {
    .visible = false,
    .named = false,
  },
  [aux_sym_number_token1] = {
    .visible = false,
    .named = false,
  },
  [anon_sym_DOT] = {
    .visible = true,
    .named = false,
  },
  [sym_string_value] = {
    .visible = true,
    .named = true,
  },
  [anon_sym_DQUOTE] = {
    .visible = true,
    .named = false,
  },
  [sym_id_name] = {
    .visible = true,
    .named = true,
  },
  [anon_sym_LBRACK] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_RBRACK] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_LBRACE] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_COMMA] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_RBRACE] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_EQ_EQ] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_BANG_EQ] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_IN] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_NOTIN] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_GT] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_GT_EQ] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_LT] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_LT_EQ] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_OR] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_AND] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_LPAREN] = {
    .visible = true,
    .named = false,
  },
  [anon_sym_RPAREN] = {
    .visible = true,
    .named = false,
  },
  [sym_source_file] = {
    .visible = true,
    .named = true,
  },
  [sym_number_value] = {
    .visible = true,
    .named = true,
  },
  [sym_number] = {
    .visible = true,
    .named = true,
  },
  [sym_string] = {
    .visible = true,
    .named = true,
  },
  [sym_id_index] = {
    .visible = true,
    .named = true,
  },
  [sym_identifier] = {
    .visible = true,
    .named = true,
  },
  [sym_array] = {
    .visible = true,
    .named = true,
  },
  [sym_compare_op] = {
    .visible = true,
    .named = true,
  },
  [sym_condition] = {
    .visible = true,
    .named = true,
  },
  [sym_logic_op] = {
    .visible = true,
    .named = true,
  },
  [sym_condition_group] = {
    .visible = true,
    .named = true,
  },
  [sym_expression] = {
    .visible = true,
    .named = true,
  },
  [aux_sym_array_repeat1] = {
    .visible = false,
    .named = false,
  },
};

static const TSSymbol ts_alias_sequences[PRODUCTION_ID_COUNT][MAX_ALIAS_SEQUENCE_LENGTH] = {
  [0] = {0},
};

static const uint16_t ts_non_terminal_alias_map[] = {
  0,
};

static const TSStateId ts_primary_state_ids[STATE_COUNT] = {
  [0] = 0,
  [1] = 1,
  [2] = 2,
  [3] = 3,
  [4] = 4,
  [5] = 5,
  [6] = 6,
  [7] = 7,
  [8] = 8,
  [9] = 9,
  [10] = 10,
  [11] = 11,
  [12] = 12,
  [13] = 13,
  [14] = 14,
  [15] = 15,
  [16] = 16,
  [17] = 17,
  [18] = 18,
  [19] = 19,
  [20] = 20,
  [21] = 21,
  [22] = 22,
  [23] = 23,
  [24] = 24,
  [25] = 25,
  [26] = 26,
  [27] = 27,
  [28] = 28,
  [29] = 29,
  [30] = 30,
  [31] = 31,
  [32] = 32,
  [33] = 33,
  [34] = 34,
  [35] = 35,
  [36] = 36,
  [37] = 37,
  [38] = 38,
  [39] = 39,
};

static bool ts_lex(TSLexer *lexer, TSStateId state) {
  START_LEXER();
  eof = lexer->eof(lexer);
  switch (state) {
    case 0:
      if (eof) ADVANCE(13);
      ADVANCE_MAP(
        '!', 3,
        '"', 19,
        '(', 36,
        ')', 37,
        ',', 24,
        '.', 16,
        '<', 32,
        '=', 4,
        '>', 30,
        'A', 7,
        'I', 8,
        'N', 10,
        'O', 11,
        '[', 21,
        ']', 22,
        '{', 23,
        '}', 25,
        '+', 15,
        '-', 15,
      );
      if (('\t' <= lookahead && lookahead <= '\r') ||
          lookahead == ' ') SKIP(0);
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(14);
      END_STATE();
    case 1:
      if (lookahead == ' ') ADVANCE(6);
      END_STATE();
    case 2:
      if (lookahead == '"') ADVANCE(19);
      if (lookahead == '(') ADVANCE(36);
      if (lookahead == '{') ADVANCE(23);
      if (lookahead == '+' ||
          lookahead == '-') ADVANCE(15);
      if (('\t' <= lookahead && lookahead <= '\r') ||
          lookahead == ' ') SKIP(2);
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(14);
      if (('A' <= lookahead && lookahead <= 'Z') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(20);
      END_STATE();
    case 3:
      if (lookahead == '=') ADVANCE(27);
      END_STATE();
    case 4:
      if (lookahead == '=') ADVANCE(26);
      END_STATE();
    case 5:
      if (lookahead == 'D') ADVANCE(35);
      END_STATE();
    case 6:
      if (lookahead == 'I') ADVANCE(9);
      END_STATE();
    case 7:
      if (lookahead == 'N') ADVANCE(5);
      END_STATE();
    case 8:
      if (lookahead == 'N') ADVANCE(28);
      END_STATE();
    case 9:
      if (lookahead == 'N') ADVANCE(29);
      END_STATE();
    case 10:
      if (lookahead == 'O') ADVANCE(12);
      END_STATE();
    case 11:
      if (lookahead == 'R') ADVANCE(34);
      END_STATE();
    case 12:
      if (lookahead == 'T') ADVANCE(1);
      END_STATE();
    case 13:
      ACCEPT_TOKEN(ts_builtin_sym_end);
      END_STATE();
    case 14:
      ACCEPT_TOKEN(aux_sym_number_value_token1);
      if (('0' <= lookahead && lookahead <= '9')) ADVANCE(14);
      END_STATE();
    case 15:
      ACCEPT_TOKEN(aux_sym_number_token1);
      END_STATE();
    case 16:
      ACCEPT_TOKEN(anon_sym_DOT);
      END_STATE();
    case 17:
      ACCEPT_TOKEN(sym_string_value);
      if (('\t' <= lookahead && lookahead <= '\r') ||
          lookahead == ' ') ADVANCE(17);
      if (lookahead != 0 &&
          lookahead != '"') ADVANCE(18);
      END_STATE();
    case 18:
      ACCEPT_TOKEN(sym_string_value);
      if (lookahead != 0 &&
          lookahead != '"') ADVANCE(18);
      END_STATE();
    case 19:
      ACCEPT_TOKEN(anon_sym_DQUOTE);
      END_STATE();
    case 20:
      ACCEPT_TOKEN(sym_id_name);
      if (('0' <= lookahead && lookahead <= '9') ||
          ('A' <= lookahead && lookahead <= 'Z') ||
          lookahead == '_' ||
          ('a' <= lookahead && lookahead <= 'z')) ADVANCE(20);
      END_STATE();
    case 21:
      ACCEPT_TOKEN(anon_sym_LBRACK);
      END_STATE();
    case 22:
      ACCEPT_TOKEN(anon_sym_RBRACK);
      END_STATE();
    case 23:
      ACCEPT_TOKEN(anon_sym_LBRACE);
      END_STATE();
    case 24:
      ACCEPT_TOKEN(anon_sym_COMMA);
      END_STATE();
    case 25:
      ACCEPT_TOKEN(anon_sym_RBRACE);
      END_STATE();
    case 26:
      ACCEPT_TOKEN(anon_sym_EQ_EQ);
      END_STATE();
    case 27:
      ACCEPT_TOKEN(anon_sym_BANG_EQ);
      END_STATE();
    case 28:
      ACCEPT_TOKEN(anon_sym_IN);
      END_STATE();
    case 29:
      ACCEPT_TOKEN(anon_sym_NOTIN);
      END_STATE();
    case 30:
      ACCEPT_TOKEN(anon_sym_GT);
      if (lookahead == '=') ADVANCE(31);
      END_STATE();
    case 31:
      ACCEPT_TOKEN(anon_sym_GT_EQ);
      END_STATE();
    case 32:
      ACCEPT_TOKEN(anon_sym_LT);
      if (lookahead == '=') ADVANCE(33);
      END_STATE();
    case 33:
      ACCEPT_TOKEN(anon_sym_LT_EQ);
      END_STATE();
    case 34:
      ACCEPT_TOKEN(anon_sym_OR);
      END_STATE();
    case 35:
      ACCEPT_TOKEN(anon_sym_AND);
      END_STATE();
    case 36:
      ACCEPT_TOKEN(anon_sym_LPAREN);
      END_STATE();
    case 37:
      ACCEPT_TOKEN(anon_sym_RPAREN);
      END_STATE();
    default:
      return false;
  }
}

static const TSLexMode ts_lex_modes[STATE_COUNT] = {
  [0] = {.lex_state = 0},
  [1] = {.lex_state = 2},
  [2] = {.lex_state = 0},
  [3] = {.lex_state = 0},
  [4] = {.lex_state = 2},
  [5] = {.lex_state = 0},
  [6] = {.lex_state = 2},
  [7] = {.lex_state = 2},
  [8] = {.lex_state = 0},
  [9] = {.lex_state = 0},
  [10] = {.lex_state = 0},
  [11] = {.lex_state = 2},
  [12] = {.lex_state = 2},
  [13] = {.lex_state = 0},
  [14] = {.lex_state = 0},
  [15] = {.lex_state = 0},
  [16] = {.lex_state = 2},
  [17] = {.lex_state = 0},
  [18] = {.lex_state = 0},
  [19] = {.lex_state = 0},
  [20] = {.lex_state = 0},
  [21] = {.lex_state = 0},
  [22] = {.lex_state = 0},
  [23] = {.lex_state = 0},
  [24] = {.lex_state = 0},
  [25] = {.lex_state = 0},
  [26] = {.lex_state = 0},
  [27] = {.lex_state = 0},
  [28] = {.lex_state = 0},
  [29] = {.lex_state = 0},
  [30] = {.lex_state = 0},
  [31] = {.lex_state = 2},
  [32] = {.lex_state = 0},
  [33] = {.lex_state = 0},
  [34] = {.lex_state = 0},
  [35] = {.lex_state = 0},
  [36] = {.lex_state = 0},
  [37] = {.lex_state = 0},
  [38] = {.lex_state = 0},
  [39] = {.lex_state = 17},
};

static const uint16_t ts_parse_table[LARGE_STATE_COUNT][SYMBOL_COUNT] = {
  [0] = {
    [ts_builtin_sym_end] = ACTIONS(1),
    [aux_sym_number_value_token1] = ACTIONS(1),
    [aux_sym_number_token1] = ACTIONS(1),
    [anon_sym_DOT] = ACTIONS(1),
    [anon_sym_DQUOTE] = ACTIONS(1),
    [anon_sym_LBRACK] = ACTIONS(1),
    [anon_sym_RBRACK] = ACTIONS(1),
    [anon_sym_LBRACE] = ACTIONS(1),
    [anon_sym_COMMA] = ACTIONS(1),
    [anon_sym_RBRACE] = ACTIONS(1),
    [anon_sym_EQ_EQ] = ACTIONS(1),
    [anon_sym_BANG_EQ] = ACTIONS(1),
    [anon_sym_IN] = ACTIONS(1),
    [anon_sym_NOTIN] = ACTIONS(1),
    [anon_sym_GT] = ACTIONS(1),
    [anon_sym_GT_EQ] = ACTIONS(1),
    [anon_sym_LT] = ACTIONS(1),
    [anon_sym_LT_EQ] = ACTIONS(1),
    [anon_sym_OR] = ACTIONS(1),
    [anon_sym_AND] = ACTIONS(1),
    [anon_sym_LPAREN] = ACTIONS(1),
    [anon_sym_RPAREN] = ACTIONS(1),
  },
  [1] = {
    [sym_source_file] = STATE(38),
    [sym_identifier] = STATE(5),
    [sym_condition] = STATE(19),
    [sym_condition_group] = STATE(19),
    [sym_expression] = STATE(37),
    [sym_id_name] = ACTIONS(3),
    [anon_sym_LPAREN] = ACTIONS(5),
  },
};

static const uint16_t ts_small_parse_table[] = {
  [0] = 3,
    ACTIONS(9), 1,
      anon_sym_LBRACK,
    ACTIONS(11), 2,
      anon_sym_GT,
      anon_sym_LT,
    ACTIONS(7), 12,
      ts_builtin_sym_end,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_EQ_EQ,
      anon_sym_BANG_EQ,
      anon_sym_IN,
      anon_sym_NOTIN,
      anon_sym_GT_EQ,
      anon_sym_LT_EQ,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [22] = 2,
    ACTIONS(15), 2,
      anon_sym_GT,
      anon_sym_LT,
    ACTIONS(13), 12,
      ts_builtin_sym_end,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_EQ_EQ,
      anon_sym_BANG_EQ,
      anon_sym_IN,
      anon_sym_NOTIN,
      anon_sym_GT_EQ,
      anon_sym_LT_EQ,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [41] = 7,
    ACTIONS(3), 1,
      sym_id_name,
    ACTIONS(17), 1,
      aux_sym_number_value_token1,
    ACTIONS(19), 1,
      aux_sym_number_token1,
    ACTIONS(21), 1,
      anon_sym_DQUOTE,
    ACTIONS(23), 1,
      anon_sym_LBRACE,
    STATE(9), 1,
      sym_number_value,
    STATE(22), 4,
      sym_number,
      sym_string,
      sym_identifier,
      sym_array,
  [66] = 3,
    STATE(4), 1,
      sym_compare_op,
    ACTIONS(27), 2,
      anon_sym_GT,
      anon_sym_LT,
    ACTIONS(25), 6,
      anon_sym_EQ_EQ,
      anon_sym_BANG_EQ,
      anon_sym_IN,
      anon_sym_NOTIN,
      anon_sym_GT_EQ,
      anon_sym_LT_EQ,
  [82] = 6,
    ACTIONS(3), 1,
      sym_id_name,
    ACTIONS(17), 1,
      aux_sym_number_value_token1,
    ACTIONS(19), 1,
      aux_sym_number_token1,
    ACTIONS(21), 1,
      anon_sym_DQUOTE,
    STATE(9), 1,
      sym_number_value,
    STATE(32), 3,
      sym_number,
      sym_string,
      sym_identifier,
  [103] = 6,
    ACTIONS(3), 1,
      sym_id_name,
    ACTIONS(17), 1,
      aux_sym_number_value_token1,
    ACTIONS(19), 1,
      aux_sym_number_token1,
    ACTIONS(21), 1,
      anon_sym_DQUOTE,
    STATE(9), 1,
      sym_number_value,
    STATE(24), 3,
      sym_number,
      sym_string,
      sym_identifier,
  [124] = 2,
    ACTIONS(31), 1,
      anon_sym_DOT,
    ACTIONS(29), 6,
      ts_builtin_sym_end,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [136] = 2,
    ACTIONS(35), 1,
      anon_sym_DOT,
    ACTIONS(33), 6,
      ts_builtin_sym_end,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [148] = 1,
    ACTIONS(37), 7,
      ts_builtin_sym_end,
      anon_sym_DOT,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [158] = 5,
    ACTIONS(3), 1,
      sym_id_name,
    ACTIONS(5), 1,
      anon_sym_LPAREN,
    STATE(5), 1,
      sym_identifier,
    STATE(33), 1,
      sym_expression,
    STATE(19), 2,
      sym_condition,
      sym_condition_group,
  [175] = 5,
    ACTIONS(3), 1,
      sym_id_name,
    ACTIONS(5), 1,
      anon_sym_LPAREN,
    STATE(5), 1,
      sym_identifier,
    STATE(21), 1,
      sym_expression,
    STATE(19), 2,
      sym_condition,
      sym_condition_group,
  [192] = 1,
    ACTIONS(39), 6,
      ts_builtin_sym_end,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [201] = 1,
    ACTIONS(41), 6,
      ts_builtin_sym_end,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [210] = 1,
    ACTIONS(43), 6,
      ts_builtin_sym_end,
      anon_sym_COMMA,
      anon_sym_RBRACE,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [219] = 1,
    ACTIONS(45), 5,
      aux_sym_number_value_token1,
      aux_sym_number_token1,
      anon_sym_DQUOTE,
      sym_id_name,
      anon_sym_LBRACE,
  [227] = 1,
    ACTIONS(47), 4,
      ts_builtin_sym_end,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [234] = 1,
    ACTIONS(49), 4,
      ts_builtin_sym_end,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [241] = 1,
    ACTIONS(51), 4,
      ts_builtin_sym_end,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [248] = 1,
    ACTIONS(53), 4,
      ts_builtin_sym_end,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [255] = 3,
    ACTIONS(57), 1,
      anon_sym_RPAREN,
    STATE(11), 1,
      sym_logic_op,
    ACTIONS(55), 2,
      anon_sym_OR,
      anon_sym_AND,
  [266] = 1,
    ACTIONS(59), 4,
      ts_builtin_sym_end,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [273] = 1,
    ACTIONS(61), 4,
      ts_builtin_sym_end,
      anon_sym_OR,
      anon_sym_AND,
      anon_sym_RPAREN,
  [280] = 3,
    ACTIONS(63), 1,
      anon_sym_COMMA,
    ACTIONS(65), 1,
      anon_sym_RBRACE,
    STATE(25), 1,
      aux_sym_array_repeat1,
  [290] = 3,
    ACTIONS(63), 1,
      anon_sym_COMMA,
    ACTIONS(67), 1,
      anon_sym_RBRACE,
    STATE(26), 1,
      aux_sym_array_repeat1,
  [300] = 3,
    ACTIONS(69), 1,
      anon_sym_COMMA,
    ACTIONS(72), 1,
      anon_sym_RBRACE,
    STATE(26), 1,
      aux_sym_array_repeat1,
  [310] = 2,
    ACTIONS(17), 1,
      aux_sym_number_value_token1,
    STATE(14), 1,
      sym_number_value,
  [317] = 2,
    ACTIONS(17), 1,
      aux_sym_number_value_token1,
    STATE(8), 1,
      sym_number_value,
  [324] = 2,
    ACTIONS(17), 1,
      aux_sym_number_value_token1,
    STATE(13), 1,
      sym_number_value,
  [331] = 2,
    ACTIONS(74), 1,
      aux_sym_number_value_token1,
    STATE(36), 1,
      sym_id_index,
  [338] = 1,
    ACTIONS(76), 2,
      sym_id_name,
      anon_sym_LPAREN,
  [343] = 1,
    ACTIONS(72), 2,
      anon_sym_COMMA,
      anon_sym_RBRACE,
  [348] = 1,
    ACTIONS(78), 1,
      anon_sym_RPAREN,
  [352] = 1,
    ACTIONS(80), 1,
      anon_sym_RBRACK,
  [356] = 1,
    ACTIONS(82), 1,
      anon_sym_DQUOTE,
  [360] = 1,
    ACTIONS(84), 1,
      anon_sym_RBRACK,
  [364] = 1,
    ACTIONS(86), 1,
      ts_builtin_sym_end,
  [368] = 1,
    ACTIONS(88), 1,
      ts_builtin_sym_end,
  [372] = 1,
    ACTIONS(90), 1,
      sym_string_value,
};

static const uint32_t ts_small_parse_table_map[] = {
  [SMALL_STATE(2)] = 0,
  [SMALL_STATE(3)] = 22,
  [SMALL_STATE(4)] = 41,
  [SMALL_STATE(5)] = 66,
  [SMALL_STATE(6)] = 82,
  [SMALL_STATE(7)] = 103,
  [SMALL_STATE(8)] = 124,
  [SMALL_STATE(9)] = 136,
  [SMALL_STATE(10)] = 148,
  [SMALL_STATE(11)] = 158,
  [SMALL_STATE(12)] = 175,
  [SMALL_STATE(13)] = 192,
  [SMALL_STATE(14)] = 201,
  [SMALL_STATE(15)] = 210,
  [SMALL_STATE(16)] = 219,
  [SMALL_STATE(17)] = 227,
  [SMALL_STATE(18)] = 234,
  [SMALL_STATE(19)] = 241,
  [SMALL_STATE(20)] = 248,
  [SMALL_STATE(21)] = 255,
  [SMALL_STATE(22)] = 266,
  [SMALL_STATE(23)] = 273,
  [SMALL_STATE(24)] = 280,
  [SMALL_STATE(25)] = 290,
  [SMALL_STATE(26)] = 300,
  [SMALL_STATE(27)] = 310,
  [SMALL_STATE(28)] = 317,
  [SMALL_STATE(29)] = 324,
  [SMALL_STATE(30)] = 331,
  [SMALL_STATE(31)] = 338,
  [SMALL_STATE(32)] = 343,
  [SMALL_STATE(33)] = 348,
  [SMALL_STATE(34)] = 352,
  [SMALL_STATE(35)] = 356,
  [SMALL_STATE(36)] = 360,
  [SMALL_STATE(37)] = 364,
  [SMALL_STATE(38)] = 368,
  [SMALL_STATE(39)] = 372,
};

static const TSParseActionEntry ts_parse_actions[] = {
  [0] = {.entry = {.count = 0, .reusable = false}},
  [1] = {.entry = {.count = 1, .reusable = false}}, RECOVER(),
  [3] = {.entry = {.count = 1, .reusable = true}}, SHIFT(2),
  [5] = {.entry = {.count = 1, .reusable = true}}, SHIFT(12),
  [7] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_identifier, 1, 0, 0),
  [9] = {.entry = {.count = 1, .reusable = true}}, SHIFT(30),
  [11] = {.entry = {.count = 1, .reusable = false}}, REDUCE(sym_identifier, 1, 0, 0),
  [13] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_identifier, 4, 0, 0),
  [15] = {.entry = {.count = 1, .reusable = false}}, REDUCE(sym_identifier, 4, 0, 0),
  [17] = {.entry = {.count = 1, .reusable = true}}, SHIFT(10),
  [19] = {.entry = {.count = 1, .reusable = true}}, SHIFT(28),
  [21] = {.entry = {.count = 1, .reusable = true}}, SHIFT(39),
  [23] = {.entry = {.count = 1, .reusable = true}}, SHIFT(7),
  [25] = {.entry = {.count = 1, .reusable = true}}, SHIFT(16),
  [27] = {.entry = {.count = 1, .reusable = false}}, SHIFT(16),
  [29] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_number, 2, 0, 0),
  [31] = {.entry = {.count = 1, .reusable = true}}, SHIFT(29),
  [33] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_number, 1, 0, 0),
  [35] = {.entry = {.count = 1, .reusable = true}}, SHIFT(27),
  [37] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_number_value, 1, 0, 0),
  [39] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_number, 4, 0, 0),
  [41] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_number, 3, 0, 0),
  [43] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_string, 3, 0, 0),
  [45] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_compare_op, 1, 0, 0),
  [47] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_expression, 3, 0, 0),
  [49] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_array, 4, 0, 0),
  [51] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_expression, 1, 0, 0),
  [53] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_array, 3, 0, 0),
  [55] = {.entry = {.count = 1, .reusable = true}}, SHIFT(31),
  [57] = {.entry = {.count = 1, .reusable = true}}, SHIFT(17),
  [59] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_condition, 3, 0, 0),
  [61] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_condition_group, 5, 0, 0),
  [63] = {.entry = {.count = 1, .reusable = true}}, SHIFT(6),
  [65] = {.entry = {.count = 1, .reusable = true}}, SHIFT(20),
  [67] = {.entry = {.count = 1, .reusable = true}}, SHIFT(18),
  [69] = {.entry = {.count = 2, .reusable = true}}, REDUCE(aux_sym_array_repeat1, 2, 0, 0), SHIFT_REPEAT(6),
  [72] = {.entry = {.count = 1, .reusable = true}}, REDUCE(aux_sym_array_repeat1, 2, 0, 0),
  [74] = {.entry = {.count = 1, .reusable = true}}, SHIFT(34),
  [76] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_logic_op, 1, 0, 0),
  [78] = {.entry = {.count = 1, .reusable = true}}, SHIFT(23),
  [80] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_id_index, 1, 0, 0),
  [82] = {.entry = {.count = 1, .reusable = true}}, SHIFT(15),
  [84] = {.entry = {.count = 1, .reusable = true}}, SHIFT(3),
  [86] = {.entry = {.count = 1, .reusable = true}}, REDUCE(sym_source_file, 1, 0, 0),
  [88] = {.entry = {.count = 1, .reusable = true}},  ACCEPT_INPUT(),
  [90] = {.entry = {.count = 1, .reusable = true}}, SHIFT(35),
};

#ifdef __cplusplus
extern "C" {
#endif
#ifdef TREE_SITTER_HIDE_SYMBOLS
#define TS_PUBLIC
#elif defined(_WIN32)
#define TS_PUBLIC __declspec(dllexport)
#else
#define TS_PUBLIC __attribute__((visibility("default")))
#endif

TS_PUBLIC const TSLanguage *tree_sitter_WDBx(void) {
  static const TSLanguage language = {
    .version = LANGUAGE_VERSION,
    .symbol_count = SYMBOL_COUNT,
    .alias_count = ALIAS_COUNT,
    .token_count = TOKEN_COUNT,
    .external_token_count = EXTERNAL_TOKEN_COUNT,
    .state_count = STATE_COUNT,
    .large_state_count = LARGE_STATE_COUNT,
    .production_id_count = PRODUCTION_ID_COUNT,
    .field_count = FIELD_COUNT,
    .max_alias_sequence_length = MAX_ALIAS_SEQUENCE_LENGTH,
    .parse_table = &ts_parse_table[0][0],
    .small_parse_table = ts_small_parse_table,
    .small_parse_table_map = ts_small_parse_table_map,
    .parse_actions = ts_parse_actions,
    .symbol_names = ts_symbol_names,
    .symbol_metadata = ts_symbol_metadata,
    .public_symbol_map = ts_symbol_map,
    .alias_map = ts_non_terminal_alias_map,
    .alias_sequences = &ts_alias_sequences[0][0],
    .lex_modes = ts_lex_modes,
    .lex_fn = ts_lex,
    .primary_state_ids = ts_primary_state_ids,
  };
  return &language;
}
#ifdef __cplusplus
}
#endif
