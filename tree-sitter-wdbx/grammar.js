/// <reference types="tree-sitter-cli/dsl" />
// @ts-check

module.exports = grammar({
    name: 'WDBx',
  
    rules: {
      source_file: $ => $.expression,
      number_value: $ => /\d+/,
      number: $ => seq(
        optional(/\+|\-/),
        $.number_value,
        optional(
            seq(
                '.',
                $.number_value
            )
        )
      ),
      string_value: $ => /[^"]*/,
      string: $ => seq('"', $.string_value, '"'),
      id_name: $ =>  /[a-zA-Z_][0-9a-zA-Z_]*/,
      id_index: $ => /\d+/,
      identifier: $ => seq(
           $.id_name,
            optional(
                seq(
                    '[',
                    $.id_index,
                    ']'
                )
            )
        ),
        array: $ => seq(
            '{',
            choice($.number, $.identifier, $.string),
            repeat(
                seq(
                    ',',
                    choice($.number, $.identifier, $.string)
                )
            ),
            '}'
        ),
        compare_op: $ => choice(
            '==',
            '!=',
            'IN',
            'NOT IN',
            '>',
            '>=',
            '<',
            '<='
        ),
        condition: $ => seq(
            $.identifier,
            $.compare_op,
            choice($.number, $.identifier, $.array, $.string)
        ),
        logic_op: $ => choice(
            'OR', 'AND'
        ),
        condition_group: $ => seq(
            '(',
            $.expression,
            $.logic_op,
            $.expression,
            ')'
        ),
        expression: $ => 
            choice(
                $.condition,
                $.condition_group,
                seq(
                    '(',
                    $.expression,
                    ')'
                )
            )
        ,
    }
  });
  