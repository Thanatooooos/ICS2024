/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  TK_LB,
  TK_RB,
  TK_INT

  /* TODO: Add more token types */

};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE}, // spaces
    {"\\+", '+'},      // plus
    {"==", TK_EQ},     // equal
    {"\\*", '*'},
    {"/", '/'},
    {"\\(", TK_LB},
    {"\\)", TK_RB},
    {"-", '-'},
    {"[+-]?(0|[1-9][0-9]*)", TK_INT},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type)
        {
        case TK_NOTYPE:
          break;
        case TK_EQ:
        case '-':
        case '/':
          tokens[nr_token].type = rules[i].token_type;
          strcpy(tokens[nr_token].str, rules[i].regex);
          nr_token++;
          break;
        case TK_LB:
        case TK_RB:
        case '+':
        case '*':
          tokens[nr_token].type = rules[i].token_type;
          strcpy(tokens[nr_token].str, rules[i].regex + 1);
          nr_token++;
          break;

        case TK_INT:
          if (substr_len > 32)
          {
            printf("Error : the number you input is too large !!! \n");
            break;
          }
          else
          {
            tokens[nr_token].type = TK_INT;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
          }
          break;
          default:
            printf("input invalid expr \n");
            return false;
        }

        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool IsOp(int token_type)
{
  return token_type == '+' || token_type == '*' || token_type == '/' || token_type == '-';
}
int GetOpWeight(int token_type)
{
  int weight = -1;
  switch (token_type)
  {
  case '+':
  case '-':
    weight = 1;
    break;
  case '*':
  case '/':
    weight = 2;
    break;
  }
  return weight;
}
bool check_parentheses(int p, int q)
{
  if (!(tokens[p].type == TK_LB) || !(tokens[q].type == TK_RB))
    return false;
  int depth = 0;
  for (int i = p; i <= q; i++)
  {
    if (tokens[i].type == TK_LB)
    {
      depth++;
    }
    if (tokens[i].type == TK_RB)
    {
      depth--;
    }

    if (depth == 0 && i < q)
    {
      return false;
    }
    if (depth < 0)
    {
      return false;
    }
  }
  return depth == 0;
}

int FindPosition(int p, int q)
{
  int MIN = 99999;
  int pos = -1;
  int depth = 0;
  for (int i = p; i <= q; i++)
  {
    if (strcmp(tokens[i].str, "(") == 0)
    {
      depth++;
      continue;
    }
    else if (strcmp(tokens[i].str, ")") == 0)
    {
      depth--;
      continue;
    }

    if (IsOp(tokens[i].type) && depth == 0)
    {
      if (GetOpWeight(tokens[i].type) <= MIN)
      {
        MIN = GetOpWeight(tokens[i].type);
        pos = i;
      }
    }
  }
  return pos;
}

int eval(int p, int q)
{
  if (p > q)
  {
    assert(0);
  }
  else if (p == q)
  {
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true)
  {
    return eval(p + 1, q - 1);
  }
  else
  {
    int pos = FindPosition(p, q);
    if (pos == -1)
      assert(0);
    int val1 = eval(p, pos - 1);
    int val2 = eval(pos + 1, q);
    int val3;
    int op = tokens[pos].type;
    switch (op)
    {
    case '+':
      val3 = val1 + val2;
      break;
    case '-':
      val3 = val1 - val2;
      break;
    case '*':
      val3 = val1 * val2;
      break;
    case '/':
      if (val2 == 0)
      {
        printf("Wrong input value !!! You are excepting a zero\n");
        assert(0);
      }
      else
        val3 = val1 / val2;
      break;
    default:
      assert(0);
    }
    return val3;
  }
}
word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  return eval(0,nr_token-1);
}
