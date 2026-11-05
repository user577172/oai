/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         asn1p_parse
#define yylex           asn1p_lex
#define yyerror         asn1p_error
#define yydebug         asn1p_debug
#define yynerrs         asn1p_nerrs
#define yylval          asn1p_lval
#define yychar          asn1p_char

/* First part of user prologue.  */
#line 3 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>

#include "asn1parser.h"

#define YYERROR_VERBOSE
#define YYDEBUG 1
#define YYFPRINTF   prefixed_fprintf

/*
 * Prefix parser debug with "PARSER: " for easier human eye scanning.
 */
static int
__attribute__((format(printf, 2, 3)))
prefixed_fprintf(FILE *f, const char *fmt, ...) {
    static int line_ended = 1;
    va_list ap;
    va_start(ap, fmt);
    if(line_ended) {
        fprintf(f, "PARSER: ");
        line_ended = 0;
    }
    size_t len = strlen(fmt);
    if(len && fmt[len-1] == '\n') {
        line_ended = 1;
    }
    int ret = vfprintf(f, fmt, ap);
    va_end(ap);
    return ret;
}

int yylex(void);
int yyerror(void **param, const char *msg);
#ifdef	YYBYACC
int yyparse(void **param);	/* byacc does not produce a prototype */
#endif
void asn1p_lexer_hack_push_opaque_state(void);
void asn1p_lexer_hack_enable_with_syntax(void);
void asn1p_lexer_hack_push_encoding_control(void);
#define	yylineno	asn1p_lineno
extern int asn1p_lineno;
const char *asn1p_parse_debug_filename;
#define ASN_FILENAME asn1p_parse_debug_filename

/*
 * Process directives as <ASN1C:RepresentAsPointer>
 */
extern int asn1p_as_pointer;

/*
 * This temporary variable is used to solve the shortcomings of 1-lookahead
 * parser.
 */
static struct AssignedIdentifier *saved_aid;

static asn1p_value_t *_convert_bitstring2binary(char *str, int base);
static void _fixup_anonymous_identifier(asn1p_expr_t *expr);
static char *_encoding_control_join(char *a, char *b);
static char *_encoding_control_cstring(char *buf, int len);

static asn1p_module_t *currentModule;

/**
 * Purpose: Preserve additional constraints on a collection's element type.
 * Original source: asn1c ASN.1 parser.
 * Version: 2026-10-11, issue #575 regression fix.
 * Parameters:
 *   collection - SEQUENCE OF or SET OF expression receiving the constraint.
 *   constraints - Constraints to add to the collection's first element.
 * Returns: Zero on success; -1 if the collection is malformed or allocation fails.
 * Exceptions: None; failures are returned to the parser action.
 * Author: asn1c maintainers.
 * History: Added on 2026-10-11 to merge nested collection constraints safely.
 * Example: Keeps both SIZE constraints in nested constrained SEQUENCE OF types.
 */
static int
_asn1p_merge_collection_element_constraints(asn1p_expr_t *collection,
                                            asn1p_constraint_t *constraints) {
	asn1p_expr_t *element;
	asn1p_constraint_t *combined;
	asn1p_constraint_t *existing;
	asn1p_constraint_t *old_parent;

	if(!collection
	|| (collection->expr_type != ASN_CONSTR_SEQUENCE_OF
	    && collection->expr_type != ASN_CONSTR_SET_OF)) {
		return -1;
	}
	element = TQ_FIRST(&collection->members);
	/*
	 * Constraints following a nested collection chain belong to its leaf
	 * element. Keep each collection's own SIZE constraint on that collection.
	 */
	while(element
	&& (element->expr_type == ASN_CONSTR_SEQUENCE_OF
	    || element->expr_type == ASN_CONSTR_SET_OF)) {
		element = TQ_FIRST(&element->members);
	}
	if(!element) return -1;
	if(!constraints) return 0;

	existing = element->constraints;
	if(!existing) {
		element->constraints = constraints;
		return 0;
	}
	if(existing->type == ACT_CA_SET) {
		return asn1p_constraint_insert(existing, constraints);
	}

	combined = asn1p_constraint_new(constraints->_lineno, constraints->module);
	if(!combined) return -1;
	combined->type = ACT_CA_SET;
	old_parent = existing->parent_ct;
	if(asn1p_constraint_insert(combined, existing)) {
		asn1p_constraint_free(combined);
		return -1;
	}
	if(asn1p_constraint_insert(combined, constraints)) {
		existing->parent_ct = old_parent;
		combined->el_count = 0;
		asn1p_constraint_free(combined);
		return -1;
	}
	element->constraints = combined;
	return 0;
}

#define	NEW_EXPR()	(asn1p_expr_new(yylineno, currentModule))

#define	checkmem(ptr)	do {						\
		if(!(ptr))						\
		return yyerror(param, "Memory failure");		\
	} while(0)

#define	CONSTRAINT_INSERT(root, constr_type, arg1, arg2) do {		\
		if(arg1->type != constr_type) {				\
			int __ret;					\
			root = asn1p_constraint_new(yylineno, currentModule);	\
			checkmem(root);					\
			root->type = constr_type;			\
			__ret = asn1p_constraint_insert(root,		\
				arg1);					\
			checkmem(__ret == 0);				\
		} else {						\
			root = arg1;					\
		}							\
		if(arg2) {						\
			int __ret					\
			= asn1p_constraint_insert(root, arg2);		\
			checkmem(__ret == 0);				\
		}							\
	} while(0)

#ifdef	AL_IMPORT
#error	AL_IMPORT DEFINED ELSEWHERE!
#endif
#define AL_IMPORT(to, where, from, field)                                      \
    do {                                                                       \
        if(!(from)) break;                                                     \
        while(TQ_FIRST(&((from)->where))) {                                    \
            TQ_ADD(&((to)->where), TQ_REMOVE(&((from)->where), field), field); \
        }                                                                      \
        assert(TQ_FIRST(&((from)->where)) == 0);                               \
    } while(0)


#line 252 "asn1p_y.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_ASN1P_ASN_P_Y_H_INCLUDED
# define YY_ASN1P_ASN_P_Y_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int asn1p_debug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOK_PPEQ = 258,                /* TOK_PPEQ  */
    TOK_VBracketLeft = 259,        /* TOK_VBracketLeft  */
    TOK_VBracketRight = 260,       /* TOK_VBracketRight  */
    TOK_whitespace = 261,          /* TOK_whitespace  */
    TOK_opaque = 262,              /* TOK_opaque  */
    TOK_bstring = 263,             /* TOK_bstring  */
    TOK_cstring = 264,             /* TOK_cstring  */
    TOK_hstring = 265,             /* TOK_hstring  */
    TOK_identifier = 266,          /* "identifier"  */
    TOK_number = 267,              /* "number"  */
    TOK_number_negative = 268,     /* "negative number"  */
    TOK_realnumber = 269,          /* TOK_realnumber  */
    TOK_tuple = 270,               /* TOK_tuple  */
    TOK_quadruple = 271,           /* TOK_quadruple  */
    TOK_typereference = 272,       /* TOK_typereference  */
    TOK_capitalreference = 273,    /* TOK_capitalreference  */
    TOK_typefieldreference = 274,  /* TOK_typefieldreference  */
    TOK_valuefieldreference = 275, /* TOK_valuefieldreference  */
    TOK_Literal = 276,             /* TOK_Literal  */
    TOK_ExtValue_BIT_STRING = 277, /* TOK_ExtValue_BIT_STRING  */
    TOK_ABSENT = 278,              /* TOK_ABSENT  */
    TOK_ABSTRACT_SYNTAX = 279,     /* TOK_ABSTRACT_SYNTAX  */
    TOK_ALL = 280,                 /* TOK_ALL  */
    TOK_ANY = 281,                 /* TOK_ANY  */
    TOK_APPLICATION = 282,         /* TOK_APPLICATION  */
    TOK_AUTOMATIC = 283,           /* TOK_AUTOMATIC  */
    TOK_BEGIN = 284,               /* TOK_BEGIN  */
    TOK_BIT = 285,                 /* TOK_BIT  */
    TOK_BMPString = 286,           /* TOK_BMPString  */
    TOK_BOOLEAN = 287,             /* TOK_BOOLEAN  */
    TOK_BY = 288,                  /* TOK_BY  */
    TOK_CHARACTER = 289,           /* TOK_CHARACTER  */
    TOK_CHOICE = 290,              /* TOK_CHOICE  */
    TOK_CLASS = 291,               /* TOK_CLASS  */
    TOK_COMPONENT = 292,           /* TOK_COMPONENT  */
    TOK_COMPONENTS = 293,          /* TOK_COMPONENTS  */
    TOK_CONSTRAINED = 294,         /* TOK_CONSTRAINED  */
    TOK_CONTAINING = 295,          /* TOK_CONTAINING  */
    TOK_DEFAULT = 296,             /* TOK_DEFAULT  */
    TOK_DEFINITIONS = 297,         /* TOK_DEFINITIONS  */
    TOK_DEFINED = 298,             /* TOK_DEFINED  */
    TOK_EMBEDDED = 299,            /* TOK_EMBEDDED  */
    TOK_ENCODED = 300,             /* TOK_ENCODED  */
    TOK_ENCODING_CONTROL = 301,    /* TOK_ENCODING_CONTROL  */
    TOK_END = 302,                 /* TOK_END  */
    TOK_ENUMERATED = 303,          /* TOK_ENUMERATED  */
    TOK_EXPLICIT = 304,            /* TOK_EXPLICIT  */
    TOK_EXPORTS = 305,             /* TOK_EXPORTS  */
    TOK_EXTENSIBILITY = 306,       /* TOK_EXTENSIBILITY  */
    TOK_FALSE = 307,               /* TOK_FALSE  */
    TOK_FROM = 308,                /* TOK_FROM  */
    TOK_GeneralizedTime = 309,     /* TOK_GeneralizedTime  */
    TOK_GeneralString = 310,       /* TOK_GeneralString  */
    TOK_GraphicString = 311,       /* TOK_GraphicString  */
    TOK_IA5String = 312,           /* TOK_IA5String  */
    TOK_IDENTIFIED = 313,          /* TOK_IDENTIFIED  */
    TOK_IDENTIFIER = 314,          /* TOK_IDENTIFIER  */
    TOK_IMPLICIT = 315,            /* TOK_IMPLICIT  */
    TOK_IMPLIED = 316,             /* TOK_IMPLIED  */
    TOK_IMPORTS = 317,             /* TOK_IMPORTS  */
    TOK_INCLUDES = 318,            /* TOK_INCLUDES  */
    TOK_INSTANCE = 319,            /* TOK_INSTANCE  */
    TOK_INSTRUCTIONS = 320,        /* TOK_INSTRUCTIONS  */
    TOK_INTEGER = 321,             /* TOK_INTEGER  */
    TOK_ISO646String = 322,        /* TOK_ISO646String  */
    TOK_MAX = 323,                 /* TOK_MAX  */
    TOK_MIN = 324,                 /* TOK_MIN  */
    TOK_MINUS_INFINITY = 325,      /* TOK_MINUS_INFINITY  */
    TOK_NULL = 326,                /* TOK_NULL  */
    TOK_NumericString = 327,       /* TOK_NumericString  */
    TOK_OBJECT = 328,              /* TOK_OBJECT  */
    TOK_ObjectDescriptor = 329,    /* TOK_ObjectDescriptor  */
    TOK_OCTET = 330,               /* TOK_OCTET  */
    TOK_OF = 331,                  /* TOK_OF  */
    TOK_OPTIONAL = 332,            /* TOK_OPTIONAL  */
    TOK_PATTERN = 333,             /* TOK_PATTERN  */
    TOK_PDV = 334,                 /* TOK_PDV  */
    TOK_PLUS_INFINITY = 335,       /* TOK_PLUS_INFINITY  */
    TOK_PRESENT = 336,             /* TOK_PRESENT  */
    TOK_PrintableString = 337,     /* TOK_PrintableString  */
    TOK_PRIVATE = 338,             /* TOK_PRIVATE  */
    TOK_REAL = 339,                /* TOK_REAL  */
    TOK_RELATIVE_OID = 340,        /* TOK_RELATIVE_OID  */
    TOK_SEQUENCE = 341,            /* TOK_SEQUENCE  */
    TOK_SET = 342,                 /* TOK_SET  */
    TOK_SIZE = 343,                /* TOK_SIZE  */
    TOK_STRING = 344,              /* TOK_STRING  */
    TOK_SYNTAX = 345,              /* TOK_SYNTAX  */
    TOK_T61String = 346,           /* TOK_T61String  */
    TOK_TAGS = 347,                /* TOK_TAGS  */
    TOK_TeletexString = 348,       /* TOK_TeletexString  */
    TOK_TRUE = 349,                /* TOK_TRUE  */
    TOK_TYPE_IDENTIFIER = 350,     /* TOK_TYPE_IDENTIFIER  */
    TOK_UNIQUE = 351,              /* TOK_UNIQUE  */
    TOK_UNIVERSAL = 352,           /* TOK_UNIVERSAL  */
    TOK_UniversalString = 353,     /* TOK_UniversalString  */
    TOK_UTCTime = 354,             /* TOK_UTCTime  */
    TOK_UTF8String = 355,          /* TOK_UTF8String  */
    TOK_VideotexString = 356,      /* TOK_VideotexString  */
    TOK_VisibleString = 357,       /* TOK_VisibleString  */
    TOK_WITH = 358,                /* TOK_WITH  */
    UTF8_BOM = 359,                /* "UTF-8 byte order mark"  */
    TOK_EXCEPT = 360,              /* TOK_EXCEPT  */
    TOK_INTERSECTION = 361,        /* TOK_INTERSECTION  */
    TOK_UNION = 362,               /* TOK_UNION  */
    TOK_TwoDots = 363,             /* ".."  */
    TOK_ThreeDots = 364,           /* "..."  */
    TOK_SUCCESSORS = 365,          /* TOK_SUCCESSORS  */
    TOK_DESCENDANTS = 366          /* TOK_DESCENDANTS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define TOK_PPEQ 258
#define TOK_VBracketLeft 259
#define TOK_VBracketRight 260
#define TOK_whitespace 261
#define TOK_opaque 262
#define TOK_bstring 263
#define TOK_cstring 264
#define TOK_hstring 265
#define TOK_identifier 266
#define TOK_number 267
#define TOK_number_negative 268
#define TOK_realnumber 269
#define TOK_tuple 270
#define TOK_quadruple 271
#define TOK_typereference 272
#define TOK_capitalreference 273
#define TOK_typefieldreference 274
#define TOK_valuefieldreference 275
#define TOK_Literal 276
#define TOK_ExtValue_BIT_STRING 277
#define TOK_ABSENT 278
#define TOK_ABSTRACT_SYNTAX 279
#define TOK_ALL 280
#define TOK_ANY 281
#define TOK_APPLICATION 282
#define TOK_AUTOMATIC 283
#define TOK_BEGIN 284
#define TOK_BIT 285
#define TOK_BMPString 286
#define TOK_BOOLEAN 287
#define TOK_BY 288
#define TOK_CHARACTER 289
#define TOK_CHOICE 290
#define TOK_CLASS 291
#define TOK_COMPONENT 292
#define TOK_COMPONENTS 293
#define TOK_CONSTRAINED 294
#define TOK_CONTAINING 295
#define TOK_DEFAULT 296
#define TOK_DEFINITIONS 297
#define TOK_DEFINED 298
#define TOK_EMBEDDED 299
#define TOK_ENCODED 300
#define TOK_ENCODING_CONTROL 301
#define TOK_END 302
#define TOK_ENUMERATED 303
#define TOK_EXPLICIT 304
#define TOK_EXPORTS 305
#define TOK_EXTENSIBILITY 306
#define TOK_FALSE 307
#define TOK_FROM 308
#define TOK_GeneralizedTime 309
#define TOK_GeneralString 310
#define TOK_GraphicString 311
#define TOK_IA5String 312
#define TOK_IDENTIFIED 313
#define TOK_IDENTIFIER 314
#define TOK_IMPLICIT 315
#define TOK_IMPLIED 316
#define TOK_IMPORTS 317
#define TOK_INCLUDES 318
#define TOK_INSTANCE 319
#define TOK_INSTRUCTIONS 320
#define TOK_INTEGER 321
#define TOK_ISO646String 322
#define TOK_MAX 323
#define TOK_MIN 324
#define TOK_MINUS_INFINITY 325
#define TOK_NULL 326
#define TOK_NumericString 327
#define TOK_OBJECT 328
#define TOK_ObjectDescriptor 329
#define TOK_OCTET 330
#define TOK_OF 331
#define TOK_OPTIONAL 332
#define TOK_PATTERN 333
#define TOK_PDV 334
#define TOK_PLUS_INFINITY 335
#define TOK_PRESENT 336
#define TOK_PrintableString 337
#define TOK_PRIVATE 338
#define TOK_REAL 339
#define TOK_RELATIVE_OID 340
#define TOK_SEQUENCE 341
#define TOK_SET 342
#define TOK_SIZE 343
#define TOK_STRING 344
#define TOK_SYNTAX 345
#define TOK_T61String 346
#define TOK_TAGS 347
#define TOK_TeletexString 348
#define TOK_TRUE 349
#define TOK_TYPE_IDENTIFIER 350
#define TOK_UNIQUE 351
#define TOK_UNIVERSAL 352
#define TOK_UniversalString 353
#define TOK_UTCTime 354
#define TOK_UTF8String 355
#define TOK_VideotexString 356
#define TOK_VisibleString 357
#define TOK_WITH 358
#define UTF8_BOM 359
#define TOK_EXCEPT 360
#define TOK_INTERSECTION 361
#define TOK_UNION 362
#define TOK_TwoDots 363
#define TOK_ThreeDots 364
#define TOK_SUCCESSORS 365
#define TOK_DESCENDANTS 366

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 183 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"

	asn1p_t			*a_grammar;
	asn1p_module_flags_e	 a_module_flags;
	asn1p_module_t		*a_module;
	asn1p_expr_type_e	 a_type;	/* ASN.1 Type */
	asn1p_expr_t		*a_expr;	/* Constructed collection */
	asn1p_constraint_t	*a_constr;	/* Constraint */
	enum asn1p_constraint_type_e	a_ctype;/* Constraint type */
	asn1p_xports_t		*a_xports;	/* IMports/EXports */
	struct AssignedIdentifier a_aid;	/* Assigned Identifier */
	asn1p_oid_t		*a_oid;		/* Object Identifier */
	asn1p_oid_arc_t		 a_oid_arc;	/* Single OID's arc */
	struct asn1p_type_tag_s	 a_tag;		/* A tag */
	asn1p_ref_t		*a_ref;		/* Reference to custom type */
	asn1p_wsyntx_t		*a_wsynt;	/* WITH SYNTAX contents */
	asn1p_wsyntx_chunk_t	*a_wchunk;	/* WITH SYNTAX chunk */
	struct asn1p_ref_component_s a_refcomp;	/* Component of a reference */
	asn1p_value_t		*a_value;	/* Number, DefinedValue, etc */
	struct asn1p_param_s	 a_parg;	/* A parameter argument */
	asn1p_paramlist_t	*a_plist;	/* A pargs list */
	struct asn1p_expr_marker_s a_marker;	/* OPTIONAL/DEFAULT */
	enum asn1p_constr_pres_e a_pres;	/* PRESENT/ABSENT/OPTIONAL */
	asn1c_integer_t		 a_int;
	double			 a_dbl;
	char	*tv_str;
	struct {
		char *buf;
		int len;
	}	tv_opaque;
	struct {
		char *name;
		struct asn1p_type_tag_s tag;
	} tv_nametag;
	/*
	 * Combined tag + XER encoding instruction prefix.
	 * Used by optTagOrEncoding to carry both in one production,
	 * avoiding the shift/reduce conflict that arises when both
	 * optTag and an explicit [BASE64] alternative start with '['.
	 */
	struct {
		struct asn1p_type_tag_s tag;
		int enc;	/* enum asn1p_encoding_control_type_e */
	} tv_tag_enc;

#line 572 "asn1p_y.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE asn1p_lval;


int asn1p_parse (void **param);


#endif /* !YY_ASN1P_ASN_P_Y_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_TOK_PPEQ = 3,                   /* TOK_PPEQ  */
  YYSYMBOL_TOK_VBracketLeft = 4,           /* TOK_VBracketLeft  */
  YYSYMBOL_TOK_VBracketRight = 5,          /* TOK_VBracketRight  */
  YYSYMBOL_TOK_whitespace = 6,             /* TOK_whitespace  */
  YYSYMBOL_TOK_opaque = 7,                 /* TOK_opaque  */
  YYSYMBOL_TOK_bstring = 8,                /* TOK_bstring  */
  YYSYMBOL_TOK_cstring = 9,                /* TOK_cstring  */
  YYSYMBOL_TOK_hstring = 10,               /* TOK_hstring  */
  YYSYMBOL_TOK_identifier = 11,            /* "identifier"  */
  YYSYMBOL_TOK_number = 12,                /* "number"  */
  YYSYMBOL_TOK_number_negative = 13,       /* "negative number"  */
  YYSYMBOL_TOK_realnumber = 14,            /* TOK_realnumber  */
  YYSYMBOL_TOK_tuple = 15,                 /* TOK_tuple  */
  YYSYMBOL_TOK_quadruple = 16,             /* TOK_quadruple  */
  YYSYMBOL_TOK_typereference = 17,         /* TOK_typereference  */
  YYSYMBOL_TOK_capitalreference = 18,      /* TOK_capitalreference  */
  YYSYMBOL_TOK_typefieldreference = 19,    /* TOK_typefieldreference  */
  YYSYMBOL_TOK_valuefieldreference = 20,   /* TOK_valuefieldreference  */
  YYSYMBOL_TOK_Literal = 21,               /* TOK_Literal  */
  YYSYMBOL_TOK_ExtValue_BIT_STRING = 22,   /* TOK_ExtValue_BIT_STRING  */
  YYSYMBOL_TOK_ABSENT = 23,                /* TOK_ABSENT  */
  YYSYMBOL_TOK_ABSTRACT_SYNTAX = 24,       /* TOK_ABSTRACT_SYNTAX  */
  YYSYMBOL_TOK_ALL = 25,                   /* TOK_ALL  */
  YYSYMBOL_TOK_ANY = 26,                   /* TOK_ANY  */
  YYSYMBOL_TOK_APPLICATION = 27,           /* TOK_APPLICATION  */
  YYSYMBOL_TOK_AUTOMATIC = 28,             /* TOK_AUTOMATIC  */
  YYSYMBOL_TOK_BEGIN = 29,                 /* TOK_BEGIN  */
  YYSYMBOL_TOK_BIT = 30,                   /* TOK_BIT  */
  YYSYMBOL_TOK_BMPString = 31,             /* TOK_BMPString  */
  YYSYMBOL_TOK_BOOLEAN = 32,               /* TOK_BOOLEAN  */
  YYSYMBOL_TOK_BY = 33,                    /* TOK_BY  */
  YYSYMBOL_TOK_CHARACTER = 34,             /* TOK_CHARACTER  */
  YYSYMBOL_TOK_CHOICE = 35,                /* TOK_CHOICE  */
  YYSYMBOL_TOK_CLASS = 36,                 /* TOK_CLASS  */
  YYSYMBOL_TOK_COMPONENT = 37,             /* TOK_COMPONENT  */
  YYSYMBOL_TOK_COMPONENTS = 38,            /* TOK_COMPONENTS  */
  YYSYMBOL_TOK_CONSTRAINED = 39,           /* TOK_CONSTRAINED  */
  YYSYMBOL_TOK_CONTAINING = 40,            /* TOK_CONTAINING  */
  YYSYMBOL_TOK_DEFAULT = 41,               /* TOK_DEFAULT  */
  YYSYMBOL_TOK_DEFINITIONS = 42,           /* TOK_DEFINITIONS  */
  YYSYMBOL_TOK_DEFINED = 43,               /* TOK_DEFINED  */
  YYSYMBOL_TOK_EMBEDDED = 44,              /* TOK_EMBEDDED  */
  YYSYMBOL_TOK_ENCODED = 45,               /* TOK_ENCODED  */
  YYSYMBOL_TOK_ENCODING_CONTROL = 46,      /* TOK_ENCODING_CONTROL  */
  YYSYMBOL_TOK_END = 47,                   /* TOK_END  */
  YYSYMBOL_TOK_ENUMERATED = 48,            /* TOK_ENUMERATED  */
  YYSYMBOL_TOK_EXPLICIT = 49,              /* TOK_EXPLICIT  */
  YYSYMBOL_TOK_EXPORTS = 50,               /* TOK_EXPORTS  */
  YYSYMBOL_TOK_EXTENSIBILITY = 51,         /* TOK_EXTENSIBILITY  */
  YYSYMBOL_TOK_FALSE = 52,                 /* TOK_FALSE  */
  YYSYMBOL_TOK_FROM = 53,                  /* TOK_FROM  */
  YYSYMBOL_TOK_GeneralizedTime = 54,       /* TOK_GeneralizedTime  */
  YYSYMBOL_TOK_GeneralString = 55,         /* TOK_GeneralString  */
  YYSYMBOL_TOK_GraphicString = 56,         /* TOK_GraphicString  */
  YYSYMBOL_TOK_IA5String = 57,             /* TOK_IA5String  */
  YYSYMBOL_TOK_IDENTIFIED = 58,            /* TOK_IDENTIFIED  */
  YYSYMBOL_TOK_IDENTIFIER = 59,            /* TOK_IDENTIFIER  */
  YYSYMBOL_TOK_IMPLICIT = 60,              /* TOK_IMPLICIT  */
  YYSYMBOL_TOK_IMPLIED = 61,               /* TOK_IMPLIED  */
  YYSYMBOL_TOK_IMPORTS = 62,               /* TOK_IMPORTS  */
  YYSYMBOL_TOK_INCLUDES = 63,              /* TOK_INCLUDES  */
  YYSYMBOL_TOK_INSTANCE = 64,              /* TOK_INSTANCE  */
  YYSYMBOL_TOK_INSTRUCTIONS = 65,          /* TOK_INSTRUCTIONS  */
  YYSYMBOL_TOK_INTEGER = 66,               /* TOK_INTEGER  */
  YYSYMBOL_TOK_ISO646String = 67,          /* TOK_ISO646String  */
  YYSYMBOL_TOK_MAX = 68,                   /* TOK_MAX  */
  YYSYMBOL_TOK_MIN = 69,                   /* TOK_MIN  */
  YYSYMBOL_TOK_MINUS_INFINITY = 70,        /* TOK_MINUS_INFINITY  */
  YYSYMBOL_TOK_NULL = 71,                  /* TOK_NULL  */
  YYSYMBOL_TOK_NumericString = 72,         /* TOK_NumericString  */
  YYSYMBOL_TOK_OBJECT = 73,                /* TOK_OBJECT  */
  YYSYMBOL_TOK_ObjectDescriptor = 74,      /* TOK_ObjectDescriptor  */
  YYSYMBOL_TOK_OCTET = 75,                 /* TOK_OCTET  */
  YYSYMBOL_TOK_OF = 76,                    /* TOK_OF  */
  YYSYMBOL_TOK_OPTIONAL = 77,              /* TOK_OPTIONAL  */
  YYSYMBOL_TOK_PATTERN = 78,               /* TOK_PATTERN  */
  YYSYMBOL_TOK_PDV = 79,                   /* TOK_PDV  */
  YYSYMBOL_TOK_PLUS_INFINITY = 80,         /* TOK_PLUS_INFINITY  */
  YYSYMBOL_TOK_PRESENT = 81,               /* TOK_PRESENT  */
  YYSYMBOL_TOK_PrintableString = 82,       /* TOK_PrintableString  */
  YYSYMBOL_TOK_PRIVATE = 83,               /* TOK_PRIVATE  */
  YYSYMBOL_TOK_REAL = 84,                  /* TOK_REAL  */
  YYSYMBOL_TOK_RELATIVE_OID = 85,          /* TOK_RELATIVE_OID  */
  YYSYMBOL_TOK_SEQUENCE = 86,              /* TOK_SEQUENCE  */
  YYSYMBOL_TOK_SET = 87,                   /* TOK_SET  */
  YYSYMBOL_TOK_SIZE = 88,                  /* TOK_SIZE  */
  YYSYMBOL_TOK_STRING = 89,                /* TOK_STRING  */
  YYSYMBOL_TOK_SYNTAX = 90,                /* TOK_SYNTAX  */
  YYSYMBOL_TOK_T61String = 91,             /* TOK_T61String  */
  YYSYMBOL_TOK_TAGS = 92,                  /* TOK_TAGS  */
  YYSYMBOL_TOK_TeletexString = 93,         /* TOK_TeletexString  */
  YYSYMBOL_TOK_TRUE = 94,                  /* TOK_TRUE  */
  YYSYMBOL_TOK_TYPE_IDENTIFIER = 95,       /* TOK_TYPE_IDENTIFIER  */
  YYSYMBOL_TOK_UNIQUE = 96,                /* TOK_UNIQUE  */
  YYSYMBOL_TOK_UNIVERSAL = 97,             /* TOK_UNIVERSAL  */
  YYSYMBOL_TOK_UniversalString = 98,       /* TOK_UniversalString  */
  YYSYMBOL_TOK_UTCTime = 99,               /* TOK_UTCTime  */
  YYSYMBOL_TOK_UTF8String = 100,           /* TOK_UTF8String  */
  YYSYMBOL_TOK_VideotexString = 101,       /* TOK_VideotexString  */
  YYSYMBOL_TOK_VisibleString = 102,        /* TOK_VisibleString  */
  YYSYMBOL_TOK_WITH = 103,                 /* TOK_WITH  */
  YYSYMBOL_UTF8_BOM = 104,                 /* "UTF-8 byte order mark"  */
  YYSYMBOL_TOK_EXCEPT = 105,               /* TOK_EXCEPT  */
  YYSYMBOL_106_ = 106,                     /* '^'  */
  YYSYMBOL_TOK_INTERSECTION = 107,         /* TOK_INTERSECTION  */
  YYSYMBOL_108_ = 108,                     /* '|'  */
  YYSYMBOL_TOK_UNION = 109,                /* TOK_UNION  */
  YYSYMBOL_TOK_TwoDots = 110,              /* ".."  */
  YYSYMBOL_TOK_ThreeDots = 111,            /* "..."  */
  YYSYMBOL_TOK_SUCCESSORS = 112,           /* TOK_SUCCESSORS  */
  YYSYMBOL_TOK_DESCENDANTS = 113,          /* TOK_DESCENDANTS  */
  YYSYMBOL_114_concrete_TypeDeclaration_ = 114, /* "concrete TypeDeclaration"  */
  YYSYMBOL_115_ = 115,                     /* '{'  */
  YYSYMBOL_116_ = 116,                     /* '}'  */
  YYSYMBOL_117_ = 117,                     /* '('  */
  YYSYMBOL_118_ = 118,                     /* ')'  */
  YYSYMBOL_119_ = 119,                     /* '.'  */
  YYSYMBOL_120_ = 120,                     /* ';'  */
  YYSYMBOL_121_ = 121,                     /* ','  */
  YYSYMBOL_122_ = 122,                     /* ':'  */
  YYSYMBOL_123_ = 123,                     /* '['  */
  YYSYMBOL_124_ = 124,                     /* ']'  */
  YYSYMBOL_125_ = 125,                     /* '!'  */
  YYSYMBOL_126_ = 126,                     /* '<'  */
  YYSYMBOL_127_ = 127,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 128,                 /* $accept  */
  YYSYMBOL_ParsedGrammar = 129,            /* ParsedGrammar  */
  YYSYMBOL_ModuleList = 130,               /* ModuleList  */
  YYSYMBOL_ModuleDefinition = 131,         /* ModuleDefinition  */
  YYSYMBOL_132_1 = 132,                    /* $@1  */
  YYSYMBOL_optObjectIdentifier = 133,      /* optObjectIdentifier  */
  YYSYMBOL_ObjectIdentifier = 134,         /* ObjectIdentifier  */
  YYSYMBOL_ObjectIdentifierBody = 135,     /* ObjectIdentifierBody  */
  YYSYMBOL_ObjectIdentifierElement = 136,  /* ObjectIdentifierElement  */
  YYSYMBOL_optModuleDefinitionFlags = 137, /* optModuleDefinitionFlags  */
  YYSYMBOL_ModuleDefinitionFlags = 138,    /* ModuleDefinitionFlags  */
  YYSYMBOL_ModuleDefinitionFlag = 139,     /* ModuleDefinitionFlag  */
  YYSYMBOL_optModuleBody = 140,            /* optModuleBody  */
  YYSYMBOL_ModuleBody = 141,               /* ModuleBody  */
  YYSYMBOL_AssignmentList = 142,           /* AssignmentList  */
  YYSYMBOL_Assignment = 143,               /* Assignment  */
  YYSYMBOL_144_2 = 144,                    /* $@2  */
  YYSYMBOL_EncodingControlBody = 145,      /* EncodingControlBody  */
  YYSYMBOL_EncodingInstructionList = 146,  /* EncodingInstructionList  */
  YYSYMBOL_EncodingInstruction = 147,      /* EncodingInstruction  */
  YYSYMBOL_EncodingControlTarget = 148,    /* EncodingControlTarget  */
  YYSYMBOL_EncodingControlTargetAtom = 149, /* EncodingControlTargetAtom  */
  YYSYMBOL_EncodingControlOptAs = 150,     /* EncodingControlOptAs  */
  YYSYMBOL_EncodingControlAsValue = 151,   /* EncodingControlAsValue  */
  YYSYMBOL_optImports = 152,               /* optImports  */
  YYSYMBOL_ImportsDefinition = 153,        /* ImportsDefinition  */
  YYSYMBOL_optImportsBundleSet = 154,      /* optImportsBundleSet  */
  YYSYMBOL_ImportsBundleSet = 155,         /* ImportsBundleSet  */
  YYSYMBOL_AssignedIdentifier = 156,       /* AssignedIdentifier  */
  YYSYMBOL_ImportsBundle = 157,            /* ImportsBundle  */
  YYSYMBOL_ImportsBundleInt = 158,         /* ImportsBundleInt  */
  YYSYMBOL_ImportsList = 159,              /* ImportsList  */
  YYSYMBOL_ImportsElement = 160,           /* ImportsElement  */
  YYSYMBOL_ImportSelectionOption = 161,    /* ImportSelectionOption  */
  YYSYMBOL_optExports = 162,               /* optExports  */
  YYSYMBOL_ExportsDefinition = 163,        /* ExportsDefinition  */
  YYSYMBOL_ExportsBody = 164,              /* ExportsBody  */
  YYSYMBOL_ExportsElement = 165,           /* ExportsElement  */
  YYSYMBOL_ValueSet = 166,                 /* ValueSet  */
  YYSYMBOL_ValueSetTypeAssignment = 167,   /* ValueSetTypeAssignment  */
  YYSYMBOL_DefinedType = 168,              /* DefinedType  */
  YYSYMBOL_DataTypeReference = 169,        /* DataTypeReference  */
  YYSYMBOL_ParameterArgumentList = 170,    /* ParameterArgumentList  */
  YYSYMBOL_ParameterArgumentName = 171,    /* ParameterArgumentName  */
  YYSYMBOL_ActualParameterList = 172,      /* ActualParameterList  */
  YYSYMBOL_ActualParameter = 173,          /* ActualParameter  */
  YYSYMBOL_optComponentTypeLists = 174,    /* optComponentTypeLists  */
  YYSYMBOL_ComponentTypeLists = 175,       /* ComponentTypeLists  */
  YYSYMBOL_ComponentType = 176,            /* ComponentType  */
  YYSYMBOL_AlternativeTypeLists = 177,     /* AlternativeTypeLists  */
  YYSYMBOL_AlternativeType = 178,          /* AlternativeType  */
  YYSYMBOL_ObjectClass = 179,              /* ObjectClass  */
  YYSYMBOL_optUNIQUE = 180,                /* optUNIQUE  */
  YYSYMBOL_FieldSpec = 181,                /* FieldSpec  */
  YYSYMBOL_ClassField = 182,               /* ClassField  */
  YYSYMBOL_optWithSyntax = 183,            /* optWithSyntax  */
  YYSYMBOL_WithSyntax = 184,               /* WithSyntax  */
  YYSYMBOL_185_3 = 185,                    /* $@3  */
  YYSYMBOL_WithSyntaxList = 186,           /* WithSyntaxList  */
  YYSYMBOL_WithSyntaxToken = 187,          /* WithSyntaxToken  */
  YYSYMBOL_ExtensionAndException = 188,    /* ExtensionAndException  */
  YYSYMBOL_Type = 189,                     /* Type  */
  YYSYMBOL_TaggedType = 190,               /* TaggedType  */
  YYSYMBOL_DefinedUntaggedType = 191,      /* DefinedUntaggedType  */
  YYSYMBOL_UntaggedType = 192,             /* UntaggedType  */
  YYSYMBOL_MaybeIndirectTaggedType = 193,  /* MaybeIndirectTaggedType  */
  YYSYMBOL_NSTD_IndirectMarker = 194,      /* NSTD_IndirectMarker  */
  YYSYMBOL_MaybeIndirectTypeDeclaration = 195, /* MaybeIndirectTypeDeclaration  */
  YYSYMBOL_TypeDeclaration = 196,          /* TypeDeclaration  */
  YYSYMBOL_ConcreteTypeDeclaration = 197,  /* ConcreteTypeDeclaration  */
  YYSYMBOL_ComplexTypeReference = 198,     /* ComplexTypeReference  */
  YYSYMBOL_ComplexTypeReferenceAmpList = 199, /* ComplexTypeReferenceAmpList  */
  YYSYMBOL_ComplexTypeReferenceElement = 200, /* ComplexTypeReferenceElement  */
  YYSYMBOL_PrimitiveFieldReference = 201,  /* PrimitiveFieldReference  */
  YYSYMBOL_FieldName = 202,                /* FieldName  */
  YYSYMBOL_DefinedObjectClass = 203,       /* DefinedObjectClass  */
  YYSYMBOL_ValueAssignment = 204,          /* ValueAssignment  */
  YYSYMBOL_Value = 205,                    /* Value  */
  YYSYMBOL_206_4 = 206,                    /* $@4  */
  YYSYMBOL_SimpleValue = 207,              /* SimpleValue  */
  YYSYMBOL_DefinedValue = 208,             /* DefinedValue  */
  YYSYMBOL_RestrictedCharacterStringValue = 209, /* RestrictedCharacterStringValue  */
  YYSYMBOL_Opaque = 210,                   /* Opaque  */
  YYSYMBOL_OpaqueFirstToken = 211,         /* OpaqueFirstToken  */
  YYSYMBOL_BasicTypeId = 212,              /* BasicTypeId  */
  YYSYMBOL_BasicTypeId_UniverationCompatible = 213, /* BasicTypeId_UniverationCompatible  */
  YYSYMBOL_BuiltinType = 214,              /* BuiltinType  */
  YYSYMBOL_BasicString = 215,              /* BasicString  */
  YYSYMBOL_UnionMark = 216,                /* UnionMark  */
  YYSYMBOL_IntersectionMark = 217,         /* IntersectionMark  */
  YYSYMBOL_optConstraint = 218,            /* optConstraint  */
  YYSYMBOL_optManyConstraints = 219,       /* optManyConstraints  */
  YYSYMBOL_optSizeOrConstraint = 220,      /* optSizeOrConstraint  */
  YYSYMBOL_Constraint = 221,               /* Constraint  */
  YYSYMBOL_ManyConstraints = 222,          /* ManyConstraints  */
  YYSYMBOL_ConstraintSpec = 223,           /* ConstraintSpec  */
  YYSYMBOL_SubtypeConstraint = 224,        /* SubtypeConstraint  */
  YYSYMBOL_ElementSetSpecs = 225,          /* ElementSetSpecs  */
  YYSYMBOL_ElementSetSpec = 226,           /* ElementSetSpec  */
  YYSYMBOL_Unions = 227,                   /* Unions  */
  YYSYMBOL_Intersections = 228,            /* Intersections  */
  YYSYMBOL_IntersectionElements = 229,     /* IntersectionElements  */
  YYSYMBOL_Elements = 230,                 /* Elements  */
  YYSYMBOL_SubtypeElements = 231,          /* SubtypeElements  */
  YYSYMBOL_PermittedAlphabet = 232,        /* PermittedAlphabet  */
  YYSYMBOL_SizeConstraint = 233,           /* SizeConstraint  */
  YYSYMBOL_PatternConstraint = 234,        /* PatternConstraint  */
  YYSYMBOL_ValueRange = 235,               /* ValueRange  */
  YYSYMBOL_LowerEndValue = 236,            /* LowerEndValue  */
  YYSYMBOL_UpperEndValue = 237,            /* UpperEndValue  */
  YYSYMBOL_SingleValue = 238,              /* SingleValue  */
  YYSYMBOL_BitStringValue = 239,           /* BitStringValue  */
  YYSYMBOL_ContainedSubtype = 240,         /* ContainedSubtype  */
  YYSYMBOL_InnerTypeConstraints = 241,     /* InnerTypeConstraints  */
  YYSYMBOL_SingleTypeConstraint = 242,     /* SingleTypeConstraint  */
  YYSYMBOL_MultipleTypeConstraints = 243,  /* MultipleTypeConstraints  */
  YYSYMBOL_FullSpecification = 244,        /* FullSpecification  */
  YYSYMBOL_PartialSpecification = 245,     /* PartialSpecification  */
  YYSYMBOL_TypeConstraints = 246,          /* TypeConstraints  */
  YYSYMBOL_NamedConstraint = 247,          /* NamedConstraint  */
  YYSYMBOL_optPresenceConstraint = 248,    /* optPresenceConstraint  */
  YYSYMBOL_PresenceConstraint = 249,       /* PresenceConstraint  */
  YYSYMBOL_GeneralConstraint = 250,        /* GeneralConstraint  */
  YYSYMBOL_UserDefinedConstraint = 251,    /* UserDefinedConstraint  */
  YYSYMBOL_252_5 = 252,                    /* $@5  */
  YYSYMBOL_ContentsConstraint = 253,       /* ContentsConstraint  */
  YYSYMBOL_ConstraintRangeSpec = 254,      /* ConstraintRangeSpec  */
  YYSYMBOL_TableConstraint = 255,          /* TableConstraint  */
  YYSYMBOL_SimpleTableConstraint = 256,    /* SimpleTableConstraint  */
  YYSYMBOL_ComponentRelationConstraint = 257, /* ComponentRelationConstraint  */
  YYSYMBOL_AtNotationList = 258,           /* AtNotationList  */
  YYSYMBOL_AtNotationElement = 259,        /* AtNotationElement  */
  YYSYMBOL_ComponentIdList = 260,          /* ComponentIdList  */
  YYSYMBOL_optMarker = 261,                /* optMarker  */
  YYSYMBOL_Marker = 262,                   /* Marker  */
  YYSYMBOL_IdentifierList = 263,           /* IdentifierList  */
  YYSYMBOL_IdentifierElement = 264,        /* IdentifierElement  */
  YYSYMBOL_NamedNumberList = 265,          /* NamedNumberList  */
  YYSYMBOL_NamedNumber = 266,              /* NamedNumber  */
  YYSYMBOL_NamedBitList = 267,             /* NamedBitList  */
  YYSYMBOL_NamedBit = 268,                 /* NamedBit  */
  YYSYMBOL_Enumerations = 269,             /* Enumerations  */
  YYSYMBOL_UniverationList = 270,          /* UniverationList  */
  YYSYMBOL_UniverationElement = 271,       /* UniverationElement  */
  YYSYMBOL_SignedNumber = 272,             /* SignedNumber  */
  YYSYMBOL_RealValue = 273,                /* RealValue  */
  YYSYMBOL_optTag = 274,                   /* optTag  */
  YYSYMBOL_optTagOrEncoding = 275,         /* optTagOrEncoding  */
  YYSYMBOL_Tag = 276,                      /* Tag  */
  YYSYMBOL_TagTypeValue = 277,             /* TagTypeValue  */
  YYSYMBOL_TagClass = 278,                 /* TagClass  */
  YYSYMBOL_TagPlicit = 279,                /* TagPlicit  */
  YYSYMBOL_TypeRefName = 280,              /* TypeRefName  */
  YYSYMBOL_optIdentifier = 281,            /* optIdentifier  */
  YYSYMBOL_Identifier = 282,               /* Identifier  */
  YYSYMBOL_IdentifierAsReference = 283,    /* IdentifierAsReference  */
  YYSYMBOL_IdentifierAsValue = 284         /* IdentifierAsValue  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   945

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  128
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  157
/* YYNRULES -- Number of rules.  */
#define YYNRULES  374
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  578

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   367


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   125,     2,     2,     2,     2,     2,     2,
     117,   118,     2,     2,   121,     2,   119,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   122,   120,
     126,     2,     2,     2,   127,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   123,     2,   124,   106,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   115,   108,   116,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   107,   109,   110,   111,   112,   113,   114
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   519,   519,   522,   528,   533,   550,   550,   579,   580,
     584,   587,   593,   599,   608,   612,   616,   626,   627,   636,
     639,   648,   651,   654,   657,   661,   684,   685,   694,   707,
     710,   727,   734,   748,   756,   755,   792,   793,   798,   806,
     816,   845,   889,   945,   980,   991,   992,   999,  1000,  1001,
    1005,  1006,  1018,  1033,  1034,  1035,  1036,  1042,  1056,  1057,
    1060,  1070,  1077,  1078,  1081,  1086,  1093,  1094,  1098,  1102,
    1105,  1117,  1122,  1129,  1135,  1141,  1150,  1153,  1159,  1160,
    1172,  1175,  1178,  1186,  1191,  1198,  1204,  1210,  1219,  1222,
    1242,  1252,  1272,  1278,  1294,  1300,  1308,  1317,  1328,  1332,
    1340,  1348,  1356,  1367,  1372,  1379,  1380,  1388,  1396,  1419,
    1420,  1423,  1428,  1432,  1439,  1449,  1456,  1462,  1469,  1475,
    1480,  1484,  1489,  1497,  1502,  1505,  1512,  1522,  1523,  1527,
    1534,  1544,  1554,  1565,  1575,  1586,  1596,  1607,  1618,  1631,
    1632,  1639,  1638,  1647,  1651,  1658,  1662,  1665,  1669,  1675,
    1683,  1692,  1703,  1706,  1720,  1742,  1764,  1793,  1800,  1819,
    1820,  1823,  1824,  1830,  1836,  1842,  1852,  1862,  1868,  1880,
    1895,  1903,  1911,  1922,  1933,  1955,  1963,  1972,  1976,  1981,
    1990,  1995,  2000,  2008,  2031,  2041,  2042,  2043,  2043,  2051,
    2056,  2061,  2066,  2067,  2068,  2069,  2073,  2074,  2092,  2096,
    2101,  2109,  2118,  2133,  2134,  2140,  2141,  2142,  2143,  2144,
    2145,  2147,  2148,  2149,  2150,  2151,  2152,  2159,  2160,  2161,
    2165,  2171,  2176,  2181,  2186,  2191,  2200,  2201,  2205,  2209,
    2210,  2211,  2212,  2213,  2217,  2218,  2219,  2220,  2224,  2225,
    2232,  2232,  2233,  2233,  2237,  2238,  2242,  2243,  2247,  2248,
    2249,  2255,  2261,  2262,  2271,  2271,  2273,  2276,  2280,  2281,
    2287,  2298,  2299,  2305,  2306,  2312,  2313,  2320,  2321,  2327,
    2328,  2339,  2345,  2351,  2352,  2354,  2355,  2356,  2361,  2366,
    2371,  2376,  2388,  2397,  2398,  2404,  2405,  2410,  2413,  2418,
    2426,  2432,  2444,  2447,  2453,  2454,  2454,  2455,  2457,  2472,
    2477,  2483,  2497,  2498,  2502,  2505,  2508,  2516,  2517,  2518,
    2523,  2522,  2534,  2543,  2544,  2545,  2546,  2549,  2552,  2561,
    2577,  2583,  2589,  2603,  2614,  2630,  2633,  2653,  2657,  2661,
    2665,  2672,  2677,  2683,  2692,  2697,  2704,  2712,  2722,  2727,
    2734,  2742,  2752,  2767,  2772,  2779,  2786,  2794,  2802,  2809,
    2820,  2824,  2831,  2862,  2863,  2880,  2884,  2888,  2906,  2930,
    2937,  2943,  2944,  2945,  2946,  2950,  2951,  2952,  2956,  2960,
    2968,  2969,  2975,  2982,  2989
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "TOK_PPEQ",
  "TOK_VBracketLeft", "TOK_VBracketRight", "TOK_whitespace", "TOK_opaque",
  "TOK_bstring", "TOK_cstring", "TOK_hstring", "\"identifier\"",
  "\"number\"", "\"negative number\"", "TOK_realnumber", "TOK_tuple",
  "TOK_quadruple", "TOK_typereference", "TOK_capitalreference",
  "TOK_typefieldreference", "TOK_valuefieldreference", "TOK_Literal",
  "TOK_ExtValue_BIT_STRING", "TOK_ABSENT", "TOK_ABSTRACT_SYNTAX",
  "TOK_ALL", "TOK_ANY", "TOK_APPLICATION", "TOK_AUTOMATIC", "TOK_BEGIN",
  "TOK_BIT", "TOK_BMPString", "TOK_BOOLEAN", "TOK_BY", "TOK_CHARACTER",
  "TOK_CHOICE", "TOK_CLASS", "TOK_COMPONENT", "TOK_COMPONENTS",
  "TOK_CONSTRAINED", "TOK_CONTAINING", "TOK_DEFAULT", "TOK_DEFINITIONS",
  "TOK_DEFINED", "TOK_EMBEDDED", "TOK_ENCODED", "TOK_ENCODING_CONTROL",
  "TOK_END", "TOK_ENUMERATED", "TOK_EXPLICIT", "TOK_EXPORTS",
  "TOK_EXTENSIBILITY", "TOK_FALSE", "TOK_FROM", "TOK_GeneralizedTime",
  "TOK_GeneralString", "TOK_GraphicString", "TOK_IA5String",
  "TOK_IDENTIFIED", "TOK_IDENTIFIER", "TOK_IMPLICIT", "TOK_IMPLIED",
  "TOK_IMPORTS", "TOK_INCLUDES", "TOK_INSTANCE", "TOK_INSTRUCTIONS",
  "TOK_INTEGER", "TOK_ISO646String", "TOK_MAX", "TOK_MIN",
  "TOK_MINUS_INFINITY", "TOK_NULL", "TOK_NumericString", "TOK_OBJECT",
  "TOK_ObjectDescriptor", "TOK_OCTET", "TOK_OF", "TOK_OPTIONAL",
  "TOK_PATTERN", "TOK_PDV", "TOK_PLUS_INFINITY", "TOK_PRESENT",
  "TOK_PrintableString", "TOK_PRIVATE", "TOK_REAL", "TOK_RELATIVE_OID",
  "TOK_SEQUENCE", "TOK_SET", "TOK_SIZE", "TOK_STRING", "TOK_SYNTAX",
  "TOK_T61String", "TOK_TAGS", "TOK_TeletexString", "TOK_TRUE",
  "TOK_TYPE_IDENTIFIER", "TOK_UNIQUE", "TOK_UNIVERSAL",
  "TOK_UniversalString", "TOK_UTCTime", "TOK_UTF8String",
  "TOK_VideotexString", "TOK_VisibleString", "TOK_WITH",
  "\"UTF-8 byte order mark\"", "TOK_EXCEPT", "'^'", "TOK_INTERSECTION",
  "'|'", "TOK_UNION", "\"..\"", "\"...\"", "TOK_SUCCESSORS",
  "TOK_DESCENDANTS", "\"concrete TypeDeclaration\"", "'{'", "'}'", "'('",
  "')'", "'.'", "';'", "','", "':'", "'['", "']'", "'!'", "'<'", "'@'",
  "$accept", "ParsedGrammar", "ModuleList", "ModuleDefinition", "$@1",
  "optObjectIdentifier", "ObjectIdentifier", "ObjectIdentifierBody",
  "ObjectIdentifierElement", "optModuleDefinitionFlags",
  "ModuleDefinitionFlags", "ModuleDefinitionFlag", "optModuleBody",
  "ModuleBody", "AssignmentList", "Assignment", "$@2",
  "EncodingControlBody", "EncodingInstructionList", "EncodingInstruction",
  "EncodingControlTarget", "EncodingControlTargetAtom",
  "EncodingControlOptAs", "EncodingControlAsValue", "optImports",
  "ImportsDefinition", "optImportsBundleSet", "ImportsBundleSet",
  "AssignedIdentifier", "ImportsBundle", "ImportsBundleInt", "ImportsList",
  "ImportsElement", "ImportSelectionOption", "optExports",
  "ExportsDefinition", "ExportsBody", "ExportsElement", "ValueSet",
  "ValueSetTypeAssignment", "DefinedType", "DataTypeReference",
  "ParameterArgumentList", "ParameterArgumentName", "ActualParameterList",
  "ActualParameter", "optComponentTypeLists", "ComponentTypeLists",
  "ComponentType", "AlternativeTypeLists", "AlternativeType",
  "ObjectClass", "optUNIQUE", "FieldSpec", "ClassField", "optWithSyntax",
  "WithSyntax", "$@3", "WithSyntaxList", "WithSyntaxToken",
  "ExtensionAndException", "Type", "TaggedType", "DefinedUntaggedType",
  "UntaggedType", "MaybeIndirectTaggedType", "NSTD_IndirectMarker",
  "MaybeIndirectTypeDeclaration", "TypeDeclaration",
  "ConcreteTypeDeclaration", "ComplexTypeReference",
  "ComplexTypeReferenceAmpList", "ComplexTypeReferenceElement",
  "PrimitiveFieldReference", "FieldName", "DefinedObjectClass",
  "ValueAssignment", "Value", "$@4", "SimpleValue", "DefinedValue",
  "RestrictedCharacterStringValue", "Opaque", "OpaqueFirstToken",
  "BasicTypeId", "BasicTypeId_UniverationCompatible", "BuiltinType",
  "BasicString", "UnionMark", "IntersectionMark", "optConstraint",
  "optManyConstraints", "optSizeOrConstraint", "Constraint",
  "ManyConstraints", "ConstraintSpec", "SubtypeConstraint",
  "ElementSetSpecs", "ElementSetSpec", "Unions", "Intersections",
  "IntersectionElements", "Elements", "SubtypeElements",
  "PermittedAlphabet", "SizeConstraint", "PatternConstraint", "ValueRange",
  "LowerEndValue", "UpperEndValue", "SingleValue", "BitStringValue",
  "ContainedSubtype", "InnerTypeConstraints", "SingleTypeConstraint",
  "MultipleTypeConstraints", "FullSpecification", "PartialSpecification",
  "TypeConstraints", "NamedConstraint", "optPresenceConstraint",
  "PresenceConstraint", "GeneralConstraint", "UserDefinedConstraint",
  "$@5", "ContentsConstraint", "ConstraintRangeSpec", "TableConstraint",
  "SimpleTableConstraint", "ComponentRelationConstraint", "AtNotationList",
  "AtNotationElement", "ComponentIdList", "optMarker", "Marker",
  "IdentifierList", "IdentifierElement", "NamedNumberList", "NamedNumber",
  "NamedBitList", "NamedBit", "Enumerations", "UniverationList",
  "UniverationElement", "SignedNumber", "RealValue", "optTag",
  "optTagOrEncoding", "Tag", "TagTypeValue", "TagClass", "TagPlicit",
  "TypeRefName", "optIdentifier", "Identifier", "IdentifierAsReference",
  "IdentifierAsValue", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-470)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-328)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      60,  -470,  -470,   340,    65,   340,  -470,  -470,   340,  -470,
    -470,   -13,   104,    86,  -470,  -470,  -470,  -470,   110,  -470,
      59,   229,  -470,  -470,   172,   181,   192,   204,   251,   233,
     339,   229,  -470,   215,  -470,  -470,  -470,  -470,  -470,   324,
    -470,  -470,   180,    93,   320,  -470,   329,  -470,   252,  -470,
     266,  -470,   267,  -470,  -470,   161,   237,  -470,  -470,  -470,
     245,   277,  -470,   281,   245,  -470,   300,    -8,  -470,   292,
    -470,   407,   237,  -470,  -470,  -470,  -470,    11,   314,  -470,
    -470,  -470,  -470,   276,  -470,   340,   245,   317,  -470,  -470,
     -14,   843,    63,   435,  -470,   766,  -470,   131,   447,  -470,
    -470,   -13,  -470,  -470,    72,   336,  -470,  -470,   363,  -470,
    -470,   366,   379,  -470,  -470,  -470,  -470,  -470,  -470,  -470,
    -470,  -470,   398,  -470,   370,  -470,  -470,  -470,  -470,  -470,
    -470,  -470,  -470,  -470,  -470,   121,  -470,   338,  -470,  -470,
     341,   227,  -470,  -470,  -470,   449,   347,   346,   348,   351,
     425,   382,   358,   360,   400,   362,   173,   203,  -470,  -470,
     364,  -470,   365,  -470,  -470,  -470,  -470,  -470,   258,  -470,
    -470,  -470,   106,   438,    97,  -470,   403,   221,  -470,  -470,
    -470,  -470,  -470,   479,   843,   245,   245,   469,  -470,   368,
     401,  -470,   340,   357,    42,   455,   375,    44,   140,   378,
     482,   364,   127,   291,   420,  -470,  -470,   127,   421,  -470,
    -470,   364,   637,  -470,  -470,  -470,  -470,  -470,  -470,  -470,
    -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,
    -470,  -470,   380,  -470,  -470,  -470,  -470,   384,  -470,   409,
      68,  -470,  -470,  -470,   412,   240,   130,   472,   128,  -470,
     -14,  -470,  -470,  -470,  -470,  -470,   383,  -470,   346,   348,
     405,   364,   314,  -470,   369,   361,  -470,   431,   364,  -470,
    -470,   390,   387,   312,   316,  -470,   406,  -470,  -470,  -470,
    -470,  -470,   -19,   119,  -470,  -470,  -470,  -470,  -470,   394,
    -470,  -470,  -470,  -470,   144,  -470,  -470,   482,   482,   389,
      26,  -470,  -470,  -470,  -470,   314,  -470,   399,   396,  -470,
    -470,   404,  -470,   171,  -470,   411,  -470,   444,   413,    38,
    -470,  -470,    94,   314,   489,   314,   340,   414,  -470,  -470,
    -470,  -470,  -470,  -470,   415,  -470,   482,   417,   482,  -470,
     418,  -470,   198,  -470,  -470,  -470,  -470,   304,   482,   523,
     689,   689,   330,  -470,   524,  -470,  -470,   258,  -470,    94,
     120,    94,  -470,  -470,   440,   120,    94,   408,   428,   221,
    -470,  -470,  -470,   541,  -470,  -470,  -470,  -470,   364,   422,
     423,  -470,  -470,   427,  -470,  -470,   541,  -470,  -470,   541,
     541,   416,   429,   563,   408,  -470,   482,  -470,   216,  -470,
     426,   310,    44,  -470,    14,   766,   364,  -470,  -470,   140,
     310,  -470,   482,   310,   314,  -470,    46,    16,  -470,    94,
     430,  -470,   424,  -470,   433,   439,  -470,  -470,   439,  -470,
     637,  -470,   540,  -470,  -470,  -470,   482,  -470,  -470,  -470,
    -470,  -470,  -470,  -470,  -470,  -470,   482,  -470,  -470,   410,
    -470,  -470,  -470,    94,  -470,  -470,  -470,   471,  -470,  -470,
    -470,  -470,  -470,  -470,    22,  -470,  -470,  -470,  -470,   442,
     316,  -470,  -470,  -470,   441,  -470,  -470,  -470,  -470,  -470,
    -470,   482,   328,  -470,  -470,    24,    44,  -470,  -470,  -470,
    -470,   446,   448,  -470,   450,   451,  -470,    54,    46,  -470,
    -470,  -470,  -470,    13,   234,  -470,    73,  -470,  -470,  -470,
    -470,  -470,  -470,  -470,  -470,  -470,  -470,   467,   462,   243,
    -470,   364,   431,  -470,  -470,   452,   466,  -470,    62,  -470,
    -470,  -470,  -470,  -470,    83,   304,   482,   468,  -470,  -470,
     433,  -470,  -470,  -470,   482,  -470,   482,   154,  -470,  -470,
    -470,  -470,  -470,  -470,   540,   468,   482,  -470,    76,   249,
    -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,    76,
      20,  -470,  -470,  -470,    17,  -470,  -470,  -470
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,   368,   369,     0,     0,     3,     4,     6,     2,     1,
       5,     8,     0,     0,     9,   372,    16,    11,     0,    12,
      14,    17,    10,    13,     0,     0,     0,     0,     0,     0,
       0,    18,    19,     0,    25,    23,    21,    24,    22,     0,
      20,    15,    78,     0,     0,    27,    58,    79,     0,    82,
       0,    83,    85,    87,     7,    62,     0,    59,    81,    80,
       0,     0,    61,     0,    63,    64,    69,     0,    71,    73,
      75,     0,    28,    29,    33,    31,    32,   355,   355,    84,
      86,    60,    65,     0,    68,     0,     0,     0,    34,    30,
     355,     0,   361,     0,   152,     0,   356,   365,     0,    76,
      77,    66,    72,    74,     0,     0,    93,    92,     0,   226,
     205,     0,     0,   218,   214,   227,   228,   229,   217,   230,
     206,   231,     0,   239,     0,   232,   207,   210,   233,   234,
     235,   213,   236,   237,   238,     0,    96,     0,   216,   215,
      98,     0,   363,   364,   362,     0,     0,   170,   171,     0,
     167,     0,     0,   218,     0,   217,   248,   248,   160,   153,
     246,   159,    90,   220,   161,   367,   366,   359,     0,    67,
      70,    44,     0,     0,     0,    38,     0,     0,   219,   212,
     211,   209,   208,     0,     0,     0,     0,     0,   357,     0,
       0,    89,     0,     0,     0,     0,   219,   355,     0,     0,
       0,     0,   355,     0,     0,   249,   250,   355,     0,   155,
     252,   247,     0,   288,   198,   289,   350,   351,   352,   199,
     200,   190,   189,   191,   187,   184,   185,   186,   194,   195,
     192,   193,     0,   373,   374,   196,    48,    42,    49,     0,
      50,    45,    35,    39,     0,   355,   355,     0,     0,   129,
     355,    97,   102,   101,   100,    99,     0,   360,   170,   171,
       0,     0,   355,   284,     0,     0,   257,     0,   246,   291,
     287,     0,   258,   261,   263,   265,   267,   269,   273,   274,
     276,   277,     0,   271,   272,   275,   172,   178,   179,   174,
     175,   177,   173,   225,     0,   331,   333,     0,     0,   149,
       0,   119,   124,   125,   157,   355,   349,     0,   342,   343,
     348,   345,   169,     0,   334,     0,   279,     0,     0,   110,
     111,   118,   327,   355,     0,   355,   187,     0,   254,   256,
     255,   307,   309,   308,   317,   318,   370,     0,   370,   253,
     189,   108,     0,   103,   105,   106,   107,     0,     0,     0,
       0,     0,     0,    43,     0,   183,   180,     0,   329,   327,
     327,   327,   131,   328,   127,   327,   327,     0,   139,     0,
      95,    94,   358,     0,   278,   290,   280,   281,     0,     0,
       0,   154,    88,     0,   240,   241,     0,   242,   243,     0,
       0,   313,     0,     0,     0,   224,     0,   168,     0,   338,
       0,     0,   355,   162,   355,     0,   246,   123,   222,     0,
       0,   221,     0,     0,   355,   163,   355,   355,   116,   327,
       0,   312,     0,   251,     0,   353,   371,   164,   353,    91,
       0,   203,   188,   201,   204,   197,     0,    56,    54,    53,
      55,    51,    57,    52,    47,    46,     0,   330,   136,     0,
     135,   137,   128,   327,   133,   134,   138,     0,   126,   140,
     130,   262,   294,   292,     0,   293,   295,   296,   270,   259,
     264,   266,   268,   314,   315,   286,   282,   285,   176,   332,
     223,     0,     0,   150,   151,     0,   355,   120,   158,   156,
     344,     0,     0,   335,     0,     0,   117,     0,   355,   112,
     115,   310,   319,     0,     0,   321,   361,   157,   354,   157,
     104,   202,    40,    41,   181,   182,   132,     0,     0,     0,
     299,   244,     0,   316,   339,     0,     0,   122,     0,   347,
     346,   337,   336,   114,     0,     0,     0,   323,   325,   320,
       0,   165,   166,   141,     0,   297,     0,   302,   245,   260,
     340,   341,   121,   113,   311,   324,     0,   322,     0,     0,
     300,   305,   306,   304,   301,   303,   326,   145,   146,     0,
       0,   143,   147,   298,     0,   142,   144,   148
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -470,  -470,   562,   290,  -470,  -470,   484,  -470,   568,  -470,
    -470,   557,  -470,  -470,  -470,   517,  -470,  -470,  -470,   432,
    -470,   238,  -470,   241,  -470,  -470,  -470,  -470,  -470,   527,
    -470,  -470,   509,  -470,  -470,  -470,  -470,   536,   453,  -470,
     -92,  -470,  -470,   419,  -470,   167,   391,  -367,   183,  -383,
     197,   352,  -470,  -470,   236,  -470,  -470,  -470,    39,  -469,
    -192,   -28,  -470,  -470,   512,  -193,  -470,  -126,   206,  -470,
     434,  -470,   219,  -178,   371,   372,  -470,  -151,  -470,  -202,
    -205,  -470,    74,  -470,   -30,  -470,  -470,    81,  -470,  -470,
    -470,  -256,   457,  -155,  -470,  -470,  -470,   436,  -258,  -470,
     230,   231,  -175,  -470,  -470,   279,  -470,  -470,  -470,  -470,
     228,  -470,  -470,  -470,  -470,  -470,  -470,  -470,    78,    77,
    -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,  -470,
    -470,    84,    89,  -290,  -470,  -470,   232,  -470,   214,  -470,
     146,  -470,  -470,   223,  -177,  -470,   202,   -43,   -57,  -470,
    -470,  -470,     8,   298,   -12,  -470,  -378
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     4,     5,     6,    11,    13,    14,    18,    19,    30,
      31,    32,    44,    45,    72,    73,   104,   173,   174,   175,
     240,   241,   353,   441,    56,    57,    63,    64,   170,    65,
      66,    67,    68,    84,    46,    47,    50,    51,   341,    74,
     268,    75,   135,   136,   342,   343,   318,   319,   320,   300,
     301,   106,   453,   248,   249,   458,   459,   558,   570,   571,
     321,    93,    94,   269,   344,   322,   405,   406,   160,   161,
     162,   289,   290,   572,   360,   361,    76,   270,   347,   226,
     227,   228,   432,   433,   163,   138,   164,   139,   386,   389,
     547,   209,   204,   210,   211,   327,   328,   271,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   476,
     283,   229,   284,   285,   463,   465,   466,   467,   519,   520,
     564,   565,   330,   331,   535,   332,   393,   333,   334,   335,
     504,   505,   537,   362,   363,   294,   295,   313,   314,   398,
     399,   307,   308,   309,   230,   231,   507,   304,    96,    97,
     145,   167,   232,   425,   233,   234,   235
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      20,   205,   205,   158,   303,   302,    20,   346,     7,   380,
     345,     7,   381,     7,    90,   291,     7,   225,   486,   485,
     498,   310,   105,   567,    15,    15,   567,    15,   402,   527,
     402,    53,   418,    15,    95,    95,   287,   288,   568,   287,
     288,   568,   416,    70,    78,    85,   316,    95,    53,   497,
      98,    52,    70,    15,   317,    15,   339,    15,   416,   533,
      78,   137,   107,    69,    77,     9,   402,   552,    52,   448,
     450,   451,    69,   171,    70,   454,   455,     1,     2,   350,
      77,   141,   567,    15,   317,   351,   521,   416,   553,   172,
     142,   391,   176,   101,    69,   287,   288,   568,   171,   140,
     142,   576,    12,   528,    15,   576,   374,   392,    15,    92,
       1,     2,   407,    86,   172,    15,    16,   236,    48,   -36,
     158,    15,    16,   237,   238,   299,    91,   299,    21,   500,
     419,   534,   536,   518,    92,   357,   575,    92,    15,    92,
     569,   577,   403,   569,   -37,   404,   143,   404,   355,   356,
     489,    15,   216,   217,   137,   299,   143,   299,   293,   417,
     144,   357,   176,   516,     3,   317,   521,    92,   521,    92,
     144,   358,    15,   253,   255,   417,    24,   561,     1,     2,
     165,   239,   296,   404,    33,   305,   311,   352,   315,   456,
     323,   166,   140,   252,   254,   323,   483,   358,   461,   569,
     286,   292,    95,    95,   417,   491,   447,    95,   494,   303,
     302,   303,   302,    49,    62,   472,   291,   359,   364,    95,
      17,   496,   371,   462,   484,   346,    22,   -26,   345,  -283,
      43,   562,   310,   492,   375,   563,   495,   183,   299,   449,
     245,   246,   184,  -109,   368,  -283,    34,    25,    15,   369,
      92,   306,   377,    92,     1,     2,    15,    26,   355,   356,
     395,   201,     1,     2,   549,   396,   213,   214,   215,    15,
     216,   217,   218,   219,   220,     1,     2,   526,    27,   247,
      28,   357,    95,    71,    35,   397,   400,   411,   202,    29,
     203,   201,   412,   303,   302,    10,    36,   421,    10,   213,
     214,   215,    15,   216,   217,   218,   219,   220,   258,   259,
     221,   431,    37,   158,   429,    15,   260,   358,   207,   430,
     203,    15,   216,   217,   426,    38,   426,     1,     2,   222,
     324,   325,   480,    41,   422,   434,   435,   481,   158,    15,
     525,   236,    39,   221,   261,     1,     2,   444,   238,   187,
     539,   188,   223,    42,   262,   540,  -327,     1,     2,   545,
     263,  -327,   222,    92,   546,   573,   548,    54,   508,   264,
     546,   508,    58,   224,     1,     2,   287,   288,   376,   201,
      15,   541,    61,   542,   296,   223,    59,    60,    99,   100,
     305,    55,   305,    80,   265,   147,   148,   311,   378,   379,
     315,    81,   266,    83,   323,   323,   326,    87,   267,   213,
     214,   215,    15,   216,   217,   218,   219,   220,   258,   259,
     384,   385,   387,   388,   512,    88,   260,   287,   288,   514,
     515,   442,   442,   103,   513,   206,   206,    92,   146,   213,
     214,   215,    15,   216,   217,   218,   219,   220,   258,   259,
     168,   177,   178,   221,   261,   179,   260,   181,   180,   182,
     185,   189,   190,   186,   262,   192,   194,   193,   195,   400,
     263,   196,   222,   197,   305,   198,   199,   200,   244,   264,
     212,   203,   250,   221,   261,   242,   323,   256,   297,   201,
     298,   538,   257,    15,   262,   223,   336,   338,   349,   348,
     263,   354,   222,   -47,   265,   367,   382,   372,   383,   264,
     373,   390,   266,   394,   401,   408,   224,   409,   267,   201,
     414,   410,   420,   434,   538,   223,   436,   446,   413,   415,
     424,   457,   423,   427,   265,  -206,   452,   464,   469,   474,
     502,   468,   473,   482,   566,   501,   224,   511,   267,   213,
     214,   215,    15,   216,   217,   218,   219,   220,   258,   259,
     503,   517,   506,   522,   529,     8,   530,   523,   531,   532,
     550,   213,   214,   215,    15,   216,   217,   218,   219,   220,
       1,     2,   543,   544,   551,   169,    23,   556,    40,    89,
     445,    82,   443,   221,   261,   102,    79,   510,   337,   191,
     499,   487,   370,   251,   262,   460,   243,   159,   574,   554,
     263,   488,   222,   478,   208,   221,   470,   365,   366,   264,
     471,   477,   559,   560,   557,   555,   493,   524,   479,   201,
     509,   475,   490,   312,   222,   223,   428,     0,     0,   329,
       0,     0,     0,     0,   265,   213,   214,   215,    15,   216,
     217,   218,   219,   220,   258,   259,   224,   223,   267,   149,
       0,     0,     0,   150,     0,     0,     0,   151,   109,   110,
       0,   111,   152,     0,     0,     0,     0,     0,   224,     0,
       0,   112,     0,     0,     0,   153,     0,     0,     0,   221,
       0,   114,   115,   116,   117,     0,     0,     0,   437,     0,
     438,   154,     0,   155,   119,     0,   439,   440,   340,   121,
     122,   123,   124,     0,     0,     0,     0,     0,     0,   125,
     109,   126,   127,   156,   157,     0,     0,     0,   128,     0,
     129,   223,     0,     0,     0,   130,   131,   132,   133,   134,
       0,     0,     0,     0,   115,   116,   117,     0,     0,     0,
       0,     0,   190,     0,     0,     0,   119,     0,     0,     0,
       0,   121,     0,   123,     0,     0,     0,     0,     0,     0,
       0,   125,     0,     0,     0,     0,     0,     0,     0,     0,
     128,     0,   129,   147,   148,     0,     0,   130,   149,   132,
     133,   134,   150,     0,     0,     0,   151,   109,   110,     0,
     111,   152,     0,     0,     0,     0,     0,     0,     0,     0,
     112,     0,     0,     0,   153,     0,     0,     0,     0,     0,
     114,   115,   116,   117,     0,     0,     0,     0,     0,     0,
     154,     0,   155,   119,     0,     0,     0,   120,   121,   122,
     123,   124,     0,     0,     0,     0,     0,     0,   125,     0,
     126,   127,   156,   157,     0,     0,     0,   128,     0,   129,
       1,     2,     0,     0,   130,   131,   132,   133,   134,     0,
       0,     0,     0,   108,   109,   110,     0,   111,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   112,     0,     0,
       0,   113,     0,     0,     0,     0,     0,   114,   115,   116,
     117,     0,     0,     0,     0,     0,     0,     0,     0,   118,
     119,     0,     0,     0,   120,   121,   122,   123,   124,     0,
       0,     0,     0,     0,     0,   125,     0,   126,   127,     0,
       0,     0,     0,     0,   128,     0,   129,     0,     0,     0,
       0,   130,   131,   132,   133,   134
};

static const yytype_int16 yycheck[] =
{
      12,   156,   157,    95,   197,   197,    18,   212,     0,   267,
     212,     3,   268,     5,     3,   193,     8,   168,     4,   402,
       4,   198,    36,     6,    11,    11,     6,    11,     4,     5,
       4,    43,   322,    11,    77,    78,    19,    20,    21,    19,
      20,    21,     4,    55,    56,    53,   201,    90,    60,   416,
      78,    43,    64,    11,    38,    11,   211,    11,     4,     5,
      72,    91,    90,    55,    56,     0,     4,     5,    60,   359,
     360,   361,    64,     1,    86,   365,   366,    17,    18,    11,
      72,    18,     6,    11,    38,    17,   464,     4,     5,    17,
      27,   110,   104,    85,    86,    19,    20,    21,     1,    91,
      27,   570,   115,   486,    11,   574,   261,   126,    11,   123,
      17,    18,   305,   121,    17,    11,    12,    11,    25,    47,
     212,    11,    12,    17,    18,   111,   115,   111,    42,   419,
     323,   498,   119,   111,   123,    41,   116,   123,    11,   123,
     123,   124,   116,   123,    47,   121,    83,   121,    18,    19,
     406,    11,    12,    13,   184,   111,    83,   111,   116,   121,
      97,    41,   174,   453,   104,    38,   544,   123,   546,   123,
      97,    77,    11,   185,   186,   121,   117,    23,    17,    18,
      49,    75,   194,   121,    12,   197,   198,   119,   200,   367,
     202,    60,   184,   185,   186,   207,   401,    77,   373,   123,
     192,   193,   245,   246,   121,   410,   357,   250,   413,   402,
     402,   404,   404,   120,    53,   390,   394,   245,   246,   262,
     116,   414,   250,   378,   401,   430,   116,    47,   430,   110,
      50,    77,   409,   410,   262,    81,   413,   116,   111,   119,
      19,    20,   121,   116,   116,   126,    65,    18,    11,   121,
     123,   111,   264,   123,    17,    18,    11,    28,    18,    19,
     116,    88,    17,    18,   522,   121,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,   482,    49,    58,
      51,    41,   325,    46,    92,   297,   298,   116,   115,    60,
     117,    88,   121,   486,   486,     5,    92,   325,     8,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      52,     7,    61,   405,   116,    11,    25,    77,   115,   121,
     117,    11,    12,    13,   336,    92,   338,    17,    18,    71,
      39,    40,   116,   118,   326,   347,   348,   121,   430,    11,
      12,    11,     3,    52,    53,    17,    18,    17,    18,   122,
     116,   124,    94,    29,    63,   121,   116,    17,    18,   116,
      69,   121,    71,   123,   121,   116,   521,    47,   425,    78,
     121,   428,   120,   115,    17,    18,    19,    20,     9,    88,
      11,   507,   115,   509,   396,    94,   120,   121,   112,   113,
     402,    62,   404,   116,   103,    17,    18,   409,    37,    38,
     412,   120,   111,   103,   416,   417,   115,   115,   117,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
     108,   109,   106,   107,   436,    18,    25,    19,    20,    19,
      20,   350,   351,   116,   446,   156,   157,   123,     3,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
       3,   115,    89,    52,    53,    89,    25,    59,    79,    89,
     122,    12,   115,   122,    63,   119,   115,   119,    43,   481,
      69,    89,    71,   115,   486,   115,    76,   115,    75,    78,
     115,   117,     3,    52,    53,    47,   498,    18,    33,    88,
     115,   503,   124,    11,    63,    94,    76,    76,    89,   119,
      69,    89,    71,   119,   103,    33,   116,   124,   121,    78,
     105,   105,   111,   119,   125,   116,   115,   121,   117,    88,
      76,   117,    33,   535,   536,    94,     3,     3,   117,   116,
     115,   103,   118,   116,   103,   117,    96,   115,   111,   110,
     116,   118,   126,   117,   556,   115,   115,     7,   117,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
     127,    90,   123,   121,   118,     3,   118,   126,   118,   118,
     118,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,   115,   121,   118,   101,    18,   119,    31,    72,
     352,    64,   351,    52,    53,    86,    60,   430,   207,   146,
     417,   404,   250,   184,    63,   369,   174,    95,   569,   535,
      69,   405,    71,   394,   157,    52,   386,   246,   246,    78,
     389,   393,   544,   546,   540,   536,   412,   481,   396,    88,
     428,    68,   409,   199,    71,    94,   338,    -1,    -1,   203,
      -1,    -1,    -1,    -1,   103,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,   115,    94,   117,    22,
      -1,    -1,    -1,    26,    -1,    -1,    -1,    30,    31,    32,
      -1,    34,    35,    -1,    -1,    -1,    -1,    -1,   115,    -1,
      -1,    44,    -1,    -1,    -1,    48,    -1,    -1,    -1,    52,
      -1,    54,    55,    56,    57,    -1,    -1,    -1,     9,    -1,
      11,    64,    -1,    66,    67,    -1,    17,    18,    71,    72,
      73,    74,    75,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      31,    84,    85,    86,    87,    -1,    -1,    -1,    91,    -1,
      93,    94,    -1,    -1,    -1,    98,    99,   100,   101,   102,
      -1,    -1,    -1,    -1,    55,    56,    57,    -1,    -1,    -1,
      -1,    -1,   115,    -1,    -1,    -1,    67,    -1,    -1,    -1,
      -1,    72,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    -1,    93,    17,    18,    -1,    -1,    98,    22,   100,
     101,   102,    26,    -1,    -1,    -1,    30,    31,    32,    -1,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,    57,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    -1,    66,    67,    -1,    -1,    -1,    71,    72,    73,
      74,    75,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      84,    85,    86,    87,    -1,    -1,    -1,    91,    -1,    93,
      17,    18,    -1,    -1,    98,    99,   100,   101,   102,    -1,
      -1,    -1,    -1,    30,    31,    32,    -1,    34,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,    -1,
      -1,    48,    -1,    -1,    -1,    -1,    -1,    54,    55,    56,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      67,    -1,    -1,    -1,    71,    72,    73,    74,    75,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    84,    85,    -1,
      -1,    -1,    -1,    -1,    91,    -1,    93,    -1,    -1,    -1,
      -1,    98,    99,   100,   101,   102
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,    17,    18,   104,   129,   130,   131,   280,   130,     0,
     131,   132,   115,   133,   134,    11,    12,   116,   135,   136,
     282,    42,   116,   136,   117,    18,    28,    49,    51,    60,
     137,   138,   139,    12,    65,    92,    92,    61,    92,     3,
     139,   118,    29,    50,   140,   141,   162,   163,    25,   120,
     164,   165,   280,   282,    47,    62,   152,   153,   120,   120,
     121,   115,    53,   154,   155,   157,   158,   159,   160,   280,
     282,    46,   142,   143,   167,   169,   204,   280,   282,   165,
     116,   120,   157,   103,   161,    53,   121,   115,    18,   143,
       3,   115,   123,   189,   190,   275,   276,   277,   189,   112,
     113,   280,   160,   116,   144,    36,   179,   189,    30,    31,
      32,    34,    44,    48,    54,    55,    56,    57,    66,    67,
      71,    72,    73,    74,    75,    82,    84,    85,    91,    93,
      98,    99,   100,   101,   102,   170,   171,   212,   213,   215,
     280,    18,    27,    83,    97,   278,     3,    17,    18,    22,
      26,    30,    35,    48,    64,    66,    86,    87,   168,   192,
     196,   197,   198,   212,   214,    49,    60,   279,     3,   134,
     156,     1,    17,   145,   146,   147,   282,   115,    89,    89,
      79,    59,    89,   116,   121,   122,   122,   122,   124,    12,
     115,   166,   119,   119,   115,    43,    89,   115,   115,    76,
     115,    88,   115,   117,   220,   221,   233,   115,   220,   219,
     221,   222,   115,     8,     9,    10,    12,    13,    14,    15,
      16,    52,    71,    94,   115,   205,   207,   208,   209,   239,
     272,   273,   280,   282,   283,   284,    11,    17,    18,    75,
     148,   149,    47,   147,    75,    19,    20,    58,   181,   182,
       3,   171,   280,   282,   280,   282,    18,   124,    17,    18,
      25,    53,    63,    69,    78,   103,   111,   117,   168,   191,
     205,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   238,   240,   241,   280,    19,    20,   199,
     200,   201,   280,   116,   263,   264,   282,    33,   115,   111,
     177,   178,   188,   193,   275,   282,   111,   269,   270,   271,
     272,   282,   198,   265,   266,   282,   221,    38,   174,   175,
     176,   188,   193,   282,    39,    40,   115,   223,   224,   225,
     250,   251,   253,   255,   256,   257,    76,   174,    76,   221,
      71,   166,   172,   173,   192,   207,   208,   206,   119,    89,
      11,    17,   119,   150,    89,    18,    19,    41,    77,   189,
     202,   203,   261,   262,   189,   202,   203,    33,   116,   121,
     179,   189,   124,   105,   221,   189,     9,   282,    37,    38,
     226,   219,   116,   121,   108,   109,   216,   106,   107,   217,
     105,   110,   126,   254,   119,   116,   121,   282,   267,   268,
     282,   125,     4,   116,   121,   194,   195,   193,   116,   121,
     117,   116,   121,   117,    76,   116,     4,   121,   261,   193,
      33,   189,   280,   118,   115,   281,   282,   116,   281,   116,
     121,     7,   210,   211,   282,   282,     3,     9,    11,    17,
      18,   151,   215,   151,    17,   149,     3,   205,   261,   119,
     261,   261,    96,   180,   261,   261,   201,   103,   183,   184,
     182,   230,   221,   242,   115,   243,   244,   245,   118,   111,
     228,   229,   230,   126,   110,    68,   237,   238,   200,   264,
     116,   121,   117,   208,   272,   177,     4,   178,   196,   219,
     271,   208,   272,   266,   208,   272,   193,   175,     4,   176,
     261,   115,   116,   127,   258,   259,   123,   274,   276,   274,
     173,     7,   282,   282,    19,    20,   261,    90,   111,   246,
     247,   284,   121,   126,   268,    12,   208,     5,   177,   118,
     118,   118,   118,     5,   175,   252,   119,   260,   282,   116,
     121,   195,   195,   115,   121,   116,   121,   218,   221,   226,
     118,   118,     5,     5,   210,   260,   119,   259,   185,   246,
     247,    23,    77,    81,   248,   249,   282,     6,    21,   123,
     186,   187,   201,   116,   186,   116,   187,   124
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   128,   129,   129,   130,   130,   132,   131,   133,   133,
     134,   134,   135,   135,   136,   136,   136,   137,   137,   138,
     138,   139,   139,   139,   139,   139,   140,   140,   141,   142,
     142,   143,   143,   143,   144,   143,   145,   145,   146,   146,
     147,   147,   147,   147,   147,   148,   148,   149,   149,   149,
     150,   150,   150,   151,   151,   151,   151,   151,   152,   152,
     153,   153,   154,   154,   155,   155,   156,   156,   157,   157,
     158,   159,   159,   160,   160,   160,   161,   161,   162,   162,
     163,   163,   163,   164,   164,   165,   165,   165,   166,   167,
     168,   168,   169,   169,   169,   169,   170,   170,   171,   171,
     171,   171,   171,   172,   172,   173,   173,   173,   173,   174,
     174,   175,   175,   175,   175,   176,   176,   176,   176,   177,
     177,   177,   177,   178,   178,   178,   179,   180,   180,   181,
     181,   182,   182,   182,   182,   182,   182,   182,   182,   183,
     183,   185,   184,   186,   186,   187,   187,   187,   187,   188,
     188,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     196,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     198,   198,   198,   198,   198,   199,   199,   200,   201,   201,
     202,   202,   202,   203,   204,   205,   205,   206,   205,   207,
     207,   207,   207,   207,   207,   207,   208,   208,   209,   209,
     209,   210,   210,   211,   211,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   213,   213,   213,
     214,   214,   214,   214,   214,   214,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     216,   216,   217,   217,   218,   218,   219,   219,   220,   220,
     220,   221,   222,   222,   223,   223,   224,   225,   225,   225,
     225,   226,   226,   227,   227,   228,   228,   229,   229,   230,
     230,   231,   231,   231,   231,   231,   231,   231,   232,   233,
     234,   234,   235,   236,   236,   237,   237,   238,   239,   239,
     240,   240,   241,   241,   242,   243,   243,   244,   245,   246,
     246,   247,   248,   248,   249,   249,   249,   250,   250,   250,
     252,   251,   253,   254,   254,   254,   254,   255,   255,   256,
     257,   258,   258,   259,   259,   260,   260,   261,   261,   262,
     262,   263,   263,   264,   265,   265,   266,   266,   267,   267,
     268,   268,   269,   270,   270,   271,   271,   271,   271,   271,
     272,   272,   273,   274,   274,   275,   275,   275,   275,   276,
     277,   278,   278,   278,   278,   279,   279,   279,   280,   280,
     281,   281,   282,   283,   284
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     2,     0,     9,     0,     1,
       3,     2,     1,     2,     1,     4,     1,     0,     1,     1,
       2,     2,     2,     2,     2,     2,     0,     1,     3,     1,
       2,     1,     1,     1,     0,     5,     0,     1,     1,     2,
       5,     5,     2,     3,     1,     1,     3,     1,     1,     1,
       0,     2,     2,     1,     1,     1,     1,     1,     0,     1,
       3,     2,     0,     1,     1,     2,     0,     1,     2,     1,
       4,     1,     3,     1,     3,     1,     2,     2,     0,     1,
       3,     3,     2,     1,     3,     1,     3,     1,     3,     4,
       1,     4,     3,     3,     6,     6,     1,     3,     1,     3,
       3,     3,     3,     1,     3,     1,     1,     1,     1,     0,
       1,     1,     3,     5,     4,     3,     2,     3,     1,     1,
       3,     5,     4,     2,     1,     1,     5,     0,     1,     1,
       3,     2,     4,     3,     3,     3,     3,     3,     3,     0,
       1,     0,     6,     1,     2,     1,     1,     1,     3,     1,
       3,     3,     1,     2,     2,     2,     3,     0,     2,     1,
       1,     1,     4,     4,     4,     6,     6,     1,     4,     3,
       1,     1,     3,     3,     3,     1,     3,     1,     1,     1,
       1,     3,     3,     1,     4,     1,     1,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     2,     1,     1,     1,     1,     1,     1,     2,
       1,     4,     4,     5,     4,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     0,     1,     0,     1,     0,     1,
       1,     3,     1,     2,     1,     1,     1,     1,     1,     3,
       5,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     3,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     3,     3,     1,     1,     1,     3,     5,     1,
       3,     3,     0,     1,     1,     1,     1,     1,     1,     1,
       0,     5,     2,     1,     2,     2,     3,     1,     1,     3,
       4,     1,     3,     2,     3,     1,     3,     0,     1,     1,
       2,     1,     3,     1,     1,     3,     4,     4,     1,     3,
       4,     4,     1,     1,     3,     1,     4,     4,     1,     1,
       1,     1,     1,     0,     1,     0,     1,     3,     5,     2,
       4,     0,     1,     1,     1,     0,     1,     1,     1,     1,
       0,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (param, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, param); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void **param)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (param);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, void **param)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, param);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, void **param)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], param);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, param); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, void **param)
{
  YY_USE (yyvaluep);
  YY_USE (param);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yykind)
    {
    case YYSYMBOL_TOK_whitespace: /* TOK_whitespace  */
#line 240 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_opaque).buf); }
#line 2150 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_opaque: /* TOK_opaque  */
#line 240 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_opaque).buf); }
#line 2156 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_bstring: /* TOK_bstring  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2162 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_cstring: /* TOK_cstring  */
#line 240 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_opaque).buf); }
#line 2168 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_hstring: /* TOK_hstring  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2174 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_identifier: /* "identifier"  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2180 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_typereference: /* TOK_typereference  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2186 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_capitalreference: /* TOK_capitalreference  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2192 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_typefieldreference: /* TOK_typefieldreference  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2198 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_valuefieldreference: /* TOK_valuefieldreference  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2204 "asn1p_y.c"
        break;

    case YYSYMBOL_TOK_Literal: /* TOK_Literal  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2210 "asn1p_y.c"
        break;

    case YYSYMBOL_114_concrete_TypeDeclaration_: /* "concrete TypeDeclaration"  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2216 "asn1p_y.c"
        break;

    case YYSYMBOL_ModuleList: /* ModuleList  */
#line 228 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_delete(((*yyvaluep).a_grammar)); }
#line 2222 "asn1p_y.c"
        break;

    case YYSYMBOL_ModuleDefinition: /* ModuleDefinition  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2228 "asn1p_y.c"
        break;

    case YYSYMBOL_optObjectIdentifier: /* optObjectIdentifier  */
#line 233 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_oid_free(((*yyvaluep).a_oid)); }
#line 2234 "asn1p_y.c"
        break;

    case YYSYMBOL_ObjectIdentifier: /* ObjectIdentifier  */
#line 233 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_oid_free(((*yyvaluep).a_oid)); }
#line 2240 "asn1p_y.c"
        break;

    case YYSYMBOL_ObjectIdentifierBody: /* ObjectIdentifierBody  */
#line 233 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_oid_free(((*yyvaluep).a_oid)); }
#line 2246 "asn1p_y.c"
        break;

    case YYSYMBOL_optModuleBody: /* optModuleBody  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2252 "asn1p_y.c"
        break;

    case YYSYMBOL_ModuleBody: /* ModuleBody  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2258 "asn1p_y.c"
        break;

    case YYSYMBOL_AssignmentList: /* AssignmentList  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2264 "asn1p_y.c"
        break;

    case YYSYMBOL_Assignment: /* Assignment  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2270 "asn1p_y.c"
        break;

    case YYSYMBOL_EncodingControlBody: /* EncodingControlBody  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2276 "asn1p_y.c"
        break;

    case YYSYMBOL_EncodingInstructionList: /* EncodingInstructionList  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2282 "asn1p_y.c"
        break;

    case YYSYMBOL_EncodingInstruction: /* EncodingInstruction  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2288 "asn1p_y.c"
        break;

    case YYSYMBOL_EncodingControlTarget: /* EncodingControlTarget  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2294 "asn1p_y.c"
        break;

    case YYSYMBOL_EncodingControlTargetAtom: /* EncodingControlTargetAtom  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2300 "asn1p_y.c"
        break;

    case YYSYMBOL_EncodingControlOptAs: /* EncodingControlOptAs  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2306 "asn1p_y.c"
        break;

    case YYSYMBOL_EncodingControlAsValue: /* EncodingControlAsValue  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2312 "asn1p_y.c"
        break;

    case YYSYMBOL_optImports: /* optImports  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2318 "asn1p_y.c"
        break;

    case YYSYMBOL_ImportsDefinition: /* ImportsDefinition  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2324 "asn1p_y.c"
        break;

    case YYSYMBOL_optImportsBundleSet: /* optImportsBundleSet  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2330 "asn1p_y.c"
        break;

    case YYSYMBOL_ImportsBundleSet: /* ImportsBundleSet  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2336 "asn1p_y.c"
        break;

    case YYSYMBOL_ImportsBundle: /* ImportsBundle  */
#line 232 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_xports_free(((*yyvaluep).a_xports)); }
#line 2342 "asn1p_y.c"
        break;

    case YYSYMBOL_ImportsBundleInt: /* ImportsBundleInt  */
#line 232 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_xports_free(((*yyvaluep).a_xports)); }
#line 2348 "asn1p_y.c"
        break;

    case YYSYMBOL_ImportsList: /* ImportsList  */
#line 232 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_xports_free(((*yyvaluep).a_xports)); }
#line 2354 "asn1p_y.c"
        break;

    case YYSYMBOL_ImportsElement: /* ImportsElement  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2360 "asn1p_y.c"
        break;

    case YYSYMBOL_optExports: /* optExports  */
#line 229 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_module_free(((*yyvaluep).a_module)); }
#line 2366 "asn1p_y.c"
        break;

    case YYSYMBOL_ExportsDefinition: /* ExportsDefinition  */
#line 232 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_xports_free(((*yyvaluep).a_xports)); }
#line 2372 "asn1p_y.c"
        break;

    case YYSYMBOL_ExportsBody: /* ExportsBody  */
#line 232 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_xports_free(((*yyvaluep).a_xports)); }
#line 2378 "asn1p_y.c"
        break;

    case YYSYMBOL_ExportsElement: /* ExportsElement  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2384 "asn1p_y.c"
        break;

    case YYSYMBOL_ValueSet: /* ValueSet  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2390 "asn1p_y.c"
        break;

    case YYSYMBOL_ValueSetTypeAssignment: /* ValueSetTypeAssignment  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2396 "asn1p_y.c"
        break;

    case YYSYMBOL_DefinedType: /* DefinedType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2402 "asn1p_y.c"
        break;

    case YYSYMBOL_DataTypeReference: /* DataTypeReference  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2408 "asn1p_y.c"
        break;

    case YYSYMBOL_ParameterArgumentList: /* ParameterArgumentList  */
#line 238 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_paramlist_free(((*yyvaluep).a_plist)); }
#line 2414 "asn1p_y.c"
        break;

    case YYSYMBOL_ActualParameterList: /* ActualParameterList  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2420 "asn1p_y.c"
        break;

    case YYSYMBOL_ActualParameter: /* ActualParameter  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2426 "asn1p_y.c"
        break;

    case YYSYMBOL_optComponentTypeLists: /* optComponentTypeLists  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2432 "asn1p_y.c"
        break;

    case YYSYMBOL_ComponentTypeLists: /* ComponentTypeLists  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2438 "asn1p_y.c"
        break;

    case YYSYMBOL_ComponentType: /* ComponentType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2444 "asn1p_y.c"
        break;

    case YYSYMBOL_AlternativeTypeLists: /* AlternativeTypeLists  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2450 "asn1p_y.c"
        break;

    case YYSYMBOL_AlternativeType: /* AlternativeType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2456 "asn1p_y.c"
        break;

    case YYSYMBOL_ObjectClass: /* ObjectClass  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2462 "asn1p_y.c"
        break;

    case YYSYMBOL_FieldSpec: /* FieldSpec  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2468 "asn1p_y.c"
        break;

    case YYSYMBOL_ClassField: /* ClassField  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2474 "asn1p_y.c"
        break;

    case YYSYMBOL_optWithSyntax: /* optWithSyntax  */
#line 235 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_wsyntx_free(((*yyvaluep).a_wsynt)); }
#line 2480 "asn1p_y.c"
        break;

    case YYSYMBOL_WithSyntax: /* WithSyntax  */
#line 235 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_wsyntx_free(((*yyvaluep).a_wsynt)); }
#line 2486 "asn1p_y.c"
        break;

    case YYSYMBOL_WithSyntaxList: /* WithSyntaxList  */
#line 235 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_wsyntx_free(((*yyvaluep).a_wsynt)); }
#line 2492 "asn1p_y.c"
        break;

    case YYSYMBOL_WithSyntaxToken: /* WithSyntaxToken  */
#line 236 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_wsyntx_chunk_free(((*yyvaluep).a_wchunk)); }
#line 2498 "asn1p_y.c"
        break;

    case YYSYMBOL_ExtensionAndException: /* ExtensionAndException  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2504 "asn1p_y.c"
        break;

    case YYSYMBOL_Type: /* Type  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2510 "asn1p_y.c"
        break;

    case YYSYMBOL_TaggedType: /* TaggedType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2516 "asn1p_y.c"
        break;

    case YYSYMBOL_DefinedUntaggedType: /* DefinedUntaggedType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2522 "asn1p_y.c"
        break;

    case YYSYMBOL_UntaggedType: /* UntaggedType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2528 "asn1p_y.c"
        break;

    case YYSYMBOL_MaybeIndirectTaggedType: /* MaybeIndirectTaggedType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2534 "asn1p_y.c"
        break;

    case YYSYMBOL_MaybeIndirectTypeDeclaration: /* MaybeIndirectTypeDeclaration  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2540 "asn1p_y.c"
        break;

    case YYSYMBOL_TypeDeclaration: /* TypeDeclaration  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2546 "asn1p_y.c"
        break;

    case YYSYMBOL_ConcreteTypeDeclaration: /* ConcreteTypeDeclaration  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2552 "asn1p_y.c"
        break;

    case YYSYMBOL_ValueAssignment: /* ValueAssignment  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2558 "asn1p_y.c"
        break;

    case YYSYMBOL_Value: /* Value  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2564 "asn1p_y.c"
        break;

    case YYSYMBOL_SimpleValue: /* SimpleValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2570 "asn1p_y.c"
        break;

    case YYSYMBOL_DefinedValue: /* DefinedValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2576 "asn1p_y.c"
        break;

    case YYSYMBOL_RestrictedCharacterStringValue: /* RestrictedCharacterStringValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2582 "asn1p_y.c"
        break;

    case YYSYMBOL_Opaque: /* Opaque  */
#line 240 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_opaque).buf); }
#line 2588 "asn1p_y.c"
        break;

    case YYSYMBOL_OpaqueFirstToken: /* OpaqueFirstToken  */
#line 240 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_opaque).buf); }
#line 2594 "asn1p_y.c"
        break;

    case YYSYMBOL_BuiltinType: /* BuiltinType  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2600 "asn1p_y.c"
        break;

    case YYSYMBOL_optConstraint: /* optConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2606 "asn1p_y.c"
        break;

    case YYSYMBOL_optManyConstraints: /* optManyConstraints  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2612 "asn1p_y.c"
        break;

    case YYSYMBOL_optSizeOrConstraint: /* optSizeOrConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2618 "asn1p_y.c"
        break;

    case YYSYMBOL_Constraint: /* Constraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2624 "asn1p_y.c"
        break;

    case YYSYMBOL_ManyConstraints: /* ManyConstraints  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2630 "asn1p_y.c"
        break;

    case YYSYMBOL_ConstraintSpec: /* ConstraintSpec  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2636 "asn1p_y.c"
        break;

    case YYSYMBOL_SubtypeConstraint: /* SubtypeConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2642 "asn1p_y.c"
        break;

    case YYSYMBOL_ElementSetSpecs: /* ElementSetSpecs  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2648 "asn1p_y.c"
        break;

    case YYSYMBOL_ElementSetSpec: /* ElementSetSpec  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2654 "asn1p_y.c"
        break;

    case YYSYMBOL_Unions: /* Unions  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2660 "asn1p_y.c"
        break;

    case YYSYMBOL_Intersections: /* Intersections  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2666 "asn1p_y.c"
        break;

    case YYSYMBOL_IntersectionElements: /* IntersectionElements  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2672 "asn1p_y.c"
        break;

    case YYSYMBOL_Elements: /* Elements  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2678 "asn1p_y.c"
        break;

    case YYSYMBOL_SubtypeElements: /* SubtypeElements  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2684 "asn1p_y.c"
        break;

    case YYSYMBOL_PermittedAlphabet: /* PermittedAlphabet  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2690 "asn1p_y.c"
        break;

    case YYSYMBOL_SizeConstraint: /* SizeConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2696 "asn1p_y.c"
        break;

    case YYSYMBOL_PatternConstraint: /* PatternConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2702 "asn1p_y.c"
        break;

    case YYSYMBOL_ValueRange: /* ValueRange  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2708 "asn1p_y.c"
        break;

    case YYSYMBOL_LowerEndValue: /* LowerEndValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2714 "asn1p_y.c"
        break;

    case YYSYMBOL_UpperEndValue: /* UpperEndValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2720 "asn1p_y.c"
        break;

    case YYSYMBOL_SingleValue: /* SingleValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2726 "asn1p_y.c"
        break;

    case YYSYMBOL_BitStringValue: /* BitStringValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2732 "asn1p_y.c"
        break;

    case YYSYMBOL_ContainedSubtype: /* ContainedSubtype  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2738 "asn1p_y.c"
        break;

    case YYSYMBOL_InnerTypeConstraints: /* InnerTypeConstraints  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2744 "asn1p_y.c"
        break;

    case YYSYMBOL_SingleTypeConstraint: /* SingleTypeConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2750 "asn1p_y.c"
        break;

    case YYSYMBOL_MultipleTypeConstraints: /* MultipleTypeConstraints  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2756 "asn1p_y.c"
        break;

    case YYSYMBOL_FullSpecification: /* FullSpecification  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2762 "asn1p_y.c"
        break;

    case YYSYMBOL_PartialSpecification: /* PartialSpecification  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2768 "asn1p_y.c"
        break;

    case YYSYMBOL_TypeConstraints: /* TypeConstraints  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2774 "asn1p_y.c"
        break;

    case YYSYMBOL_NamedConstraint: /* NamedConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2780 "asn1p_y.c"
        break;

    case YYSYMBOL_GeneralConstraint: /* GeneralConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2786 "asn1p_y.c"
        break;

    case YYSYMBOL_UserDefinedConstraint: /* UserDefinedConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2792 "asn1p_y.c"
        break;

    case YYSYMBOL_ContentsConstraint: /* ContentsConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2798 "asn1p_y.c"
        break;

    case YYSYMBOL_TableConstraint: /* TableConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2804 "asn1p_y.c"
        break;

    case YYSYMBOL_SimpleTableConstraint: /* SimpleTableConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2810 "asn1p_y.c"
        break;

    case YYSYMBOL_ComponentRelationConstraint: /* ComponentRelationConstraint  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2816 "asn1p_y.c"
        break;

    case YYSYMBOL_AtNotationList: /* AtNotationList  */
#line 231 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_constraint_free(((*yyvaluep).a_constr)); }
#line 2822 "asn1p_y.c"
        break;

    case YYSYMBOL_ComponentIdList: /* ComponentIdList  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2828 "asn1p_y.c"
        break;

    case YYSYMBOL_IdentifierList: /* IdentifierList  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2834 "asn1p_y.c"
        break;

    case YYSYMBOL_IdentifierElement: /* IdentifierElement  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2840 "asn1p_y.c"
        break;

    case YYSYMBOL_NamedNumberList: /* NamedNumberList  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2846 "asn1p_y.c"
        break;

    case YYSYMBOL_NamedNumber: /* NamedNumber  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2852 "asn1p_y.c"
        break;

    case YYSYMBOL_NamedBitList: /* NamedBitList  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2858 "asn1p_y.c"
        break;

    case YYSYMBOL_NamedBit: /* NamedBit  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2864 "asn1p_y.c"
        break;

    case YYSYMBOL_Enumerations: /* Enumerations  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2870 "asn1p_y.c"
        break;

    case YYSYMBOL_UniverationList: /* UniverationList  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2876 "asn1p_y.c"
        break;

    case YYSYMBOL_UniverationElement: /* UniverationElement  */
#line 230 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_expr_free(((*yyvaluep).a_expr)); }
#line 2882 "asn1p_y.c"
        break;

    case YYSYMBOL_SignedNumber: /* SignedNumber  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2888 "asn1p_y.c"
        break;

    case YYSYMBOL_RealValue: /* RealValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2894 "asn1p_y.c"
        break;

    case YYSYMBOL_TypeRefName: /* TypeRefName  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2900 "asn1p_y.c"
        break;

    case YYSYMBOL_optIdentifier: /* optIdentifier  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2906 "asn1p_y.c"
        break;

    case YYSYMBOL_Identifier: /* Identifier  */
#line 239 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { free(((*yyvaluep).tv_str)); }
#line 2912 "asn1p_y.c"
        break;

    case YYSYMBOL_IdentifierAsValue: /* IdentifierAsValue  */
#line 237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
            { asn1p_value_free(((*yyvaluep).a_value)); }
#line 2918 "asn1p_y.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void **param)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* ParsedGrammar: "UTF-8 byte order mark" ModuleList  */
#line 519 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		*(void **)param = (yyvsp[0].a_grammar);
	}
#line 3190 "asn1p_y.c"
    break;

  case 3: /* ParsedGrammar: ModuleList  */
#line 522 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		*(void **)param = (yyvsp[0].a_grammar);
	}
#line 3198 "asn1p_y.c"
    break;

  case 4: /* ModuleList: ModuleDefinition  */
#line 528 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                         {
		(yyval.a_grammar) = asn1p_new();
		checkmem((yyval.a_grammar));
		TQ_ADD(&((yyval.a_grammar)->modules), (yyvsp[0].a_module), mod_next);
	}
#line 3208 "asn1p_y.c"
    break;

  case 5: /* ModuleList: ModuleList ModuleDefinition  */
#line 533 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
		(yyval.a_grammar) = (yyvsp[-1].a_grammar);
		TQ_ADD(&((yyval.a_grammar)->modules), (yyvsp[0].a_module), mod_next);
	}
#line 3217 "asn1p_y.c"
    break;

  case 6: /* $@1: %empty  */
#line 550 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    { currentModule = asn1p_module_new(); }
#line 3223 "asn1p_y.c"
    break;

  case 7: /* ModuleDefinition: TypeRefName $@1 optObjectIdentifier TOK_DEFINITIONS optModuleDefinitionFlags TOK_PPEQ TOK_BEGIN optModuleBody TOK_END  */
#line 555 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {

		(yyval.a_module) = currentModule;

		if((yyvsp[-1].a_module)) {
			asn1p_module_t tmp = *((yyval.a_module));
			*((yyval.a_module)) = *((yyvsp[-1].a_module));
			*((yyvsp[-1].a_module)) = tmp;
			asn1p_module_free((yyvsp[-1].a_module));
		} else {
			/* There's a chance that a module is just plain empty */
		}

		(yyval.a_module)->ModuleName = (yyvsp[-8].tv_str);
		(yyval.a_module)->module_oid = (yyvsp[-6].a_oid);
		(yyval.a_module)->module_flags = (yyvsp[-4].a_module_flags);
	}
#line 3245 "asn1p_y.c"
    break;

  case 8: /* optObjectIdentifier: %empty  */
#line 579 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_oid) = 0; }
#line 3251 "asn1p_y.c"
    break;

  case 9: /* optObjectIdentifier: ObjectIdentifier  */
#line 580 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                           { (yyval.a_oid) = (yyvsp[0].a_oid); }
#line 3257 "asn1p_y.c"
    break;

  case 10: /* ObjectIdentifier: '{' ObjectIdentifierBody '}'  */
#line 584 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                     {
		(yyval.a_oid) = (yyvsp[-1].a_oid);
	}
#line 3265 "asn1p_y.c"
    break;

  case 11: /* ObjectIdentifier: '{' '}'  */
#line 587 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                  {
		(yyval.a_oid) = 0;
	}
#line 3273 "asn1p_y.c"
    break;

  case 12: /* ObjectIdentifierBody: ObjectIdentifierElement  */
#line 593 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                {
		(yyval.a_oid) = asn1p_oid_new();
		asn1p_oid_add_arc((yyval.a_oid), &(yyvsp[0].a_oid_arc));
		if((yyvsp[0].a_oid_arc).name)
			free((yyvsp[0].a_oid_arc).name);
	}
#line 3284 "asn1p_y.c"
    break;

  case 13: /* ObjectIdentifierBody: ObjectIdentifierBody ObjectIdentifierElement  */
#line 599 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                       {
		(yyval.a_oid) = (yyvsp[-1].a_oid);
		asn1p_oid_add_arc((yyval.a_oid), &(yyvsp[0].a_oid_arc));
		if((yyvsp[0].a_oid_arc).name)
			free((yyvsp[0].a_oid_arc).name);
	}
#line 3295 "asn1p_y.c"
    break;

  case 14: /* ObjectIdentifierElement: Identifier  */
#line 608 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {					/* iso */
		(yyval.a_oid_arc).name = (yyvsp[0].tv_str);
		(yyval.a_oid_arc).number = -1;
	}
#line 3304 "asn1p_y.c"
    break;

  case 15: /* ObjectIdentifierElement: Identifier '(' "number" ')'  */
#line 612 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                        {		/* iso(1) */
		(yyval.a_oid_arc).name = (yyvsp[-3].tv_str);
		(yyval.a_oid_arc).number = (yyvsp[-1].a_int);
	}
#line 3313 "asn1p_y.c"
    break;

  case 16: /* ObjectIdentifierElement: "number"  */
#line 616 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {					/* 1 */
		(yyval.a_oid_arc).name = 0;
		(yyval.a_oid_arc).number = (yyvsp[0].a_int);
	}
#line 3322 "asn1p_y.c"
    break;

  case 17: /* optModuleDefinitionFlags: %empty  */
#line 626 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_module_flags) = MSF_NOFLAGS; }
#line 3328 "asn1p_y.c"
    break;

  case 18: /* optModuleDefinitionFlags: ModuleDefinitionFlags  */
#line 627 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                {
		(yyval.a_module_flags) = (yyvsp[0].a_module_flags);
	}
#line 3336 "asn1p_y.c"
    break;

  case 19: /* ModuleDefinitionFlags: ModuleDefinitionFlag  */
#line 636 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             {
		(yyval.a_module_flags) = (yyvsp[0].a_module_flags);
	}
#line 3344 "asn1p_y.c"
    break;

  case 20: /* ModuleDefinitionFlags: ModuleDefinitionFlags ModuleDefinitionFlag  */
#line 639 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                     {
		(yyval.a_module_flags) = (yyvsp[-1].a_module_flags) | (yyvsp[0].a_module_flags);
	}
#line 3352 "asn1p_y.c"
    break;

  case 21: /* ModuleDefinitionFlag: TOK_EXPLICIT TOK_TAGS  */
#line 648 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              {
		(yyval.a_module_flags) = MSF_EXPLICIT_TAGS;
	}
#line 3360 "asn1p_y.c"
    break;

  case 22: /* ModuleDefinitionFlag: TOK_IMPLICIT TOK_TAGS  */
#line 651 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                {
		(yyval.a_module_flags) = MSF_IMPLICIT_TAGS;
	}
#line 3368 "asn1p_y.c"
    break;

  case 23: /* ModuleDefinitionFlag: TOK_AUTOMATIC TOK_TAGS  */
#line 654 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                 {
		(yyval.a_module_flags) = MSF_AUTOMATIC_TAGS;
	}
#line 3376 "asn1p_y.c"
    break;

  case 24: /* ModuleDefinitionFlag: TOK_EXTENSIBILITY TOK_IMPLIED  */
#line 657 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                        {
		(yyval.a_module_flags) = MSF_EXTENSIBILITY_IMPLIED;
	}
#line 3384 "asn1p_y.c"
    break;

  case 25: /* ModuleDefinitionFlag: TOK_capitalreference TOK_INSTRUCTIONS  */
#line 661 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                {
		/* X.680Amd1 specifies TAG and XER */
		if(strcmp((yyvsp[-1].tv_str), "TAG") == 0) {
			(yyval.a_module_flags) = MSF_TAG_INSTRUCTIONS;
		} else if(strcmp((yyvsp[-1].tv_str), "XER") == 0) {
			(yyval.a_module_flags) = MSF_XER_INSTRUCTIONS;
		} else if(strcmp((yyvsp[-1].tv_str), "JER") == 0) {
			(yyval.a_module_flags) = MSF_JER_INSTRUCTIONS;
		} else {
			fprintf(stderr,
				"WARNING: %s INSTRUCTIONS at %s:%d: "
				"Unrecognized encoding reference\n",
				(yyvsp[-1].tv_str), ASN_FILENAME, yylineno);
		 	(yyval.a_module_flags) = MSF_unk_INSTRUCTIONS;
		}
		free((yyvsp[-1].tv_str));
	}
#line 3406 "asn1p_y.c"
    break;

  case 26: /* optModuleBody: %empty  */
#line 684 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_module) = 0; }
#line 3412 "asn1p_y.c"
    break;

  case 27: /* optModuleBody: ModuleBody  */
#line 685 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.a_module) = (yyvsp[0].a_module);
	}
#line 3420 "asn1p_y.c"
    break;

  case 28: /* ModuleBody: optExports optImports AssignmentList  */
#line 694 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                             {
		(yyval.a_module) = asn1p_module_new();
		AL_IMPORT((yyval.a_module), exports, (yyvsp[-2].a_module), xp_next);
		AL_IMPORT((yyval.a_module), imports, (yyvsp[-1].a_module), xp_next);
		asn1p_module_move_members((yyval.a_module), (yyvsp[0].a_module));

		asn1p_module_free((yyvsp[-2].a_module));
		asn1p_module_free((yyvsp[-1].a_module));
		asn1p_module_free((yyvsp[0].a_module));
	}
#line 3435 "asn1p_y.c"
    break;

  case 29: /* AssignmentList: Assignment  */
#line 707 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {
		(yyval.a_module) = (yyvsp[0].a_module);
	}
#line 3443 "asn1p_y.c"
    break;

  case 30: /* AssignmentList: AssignmentList Assignment  */
#line 710 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                    {
		if((yyvsp[-1].a_module)) {
			(yyval.a_module) = (yyvsp[-1].a_module);
		} else {
			(yyval.a_module) = (yyvsp[0].a_module);
			break;
		}
        asn1p_module_move_members((yyval.a_module), (yyvsp[0].a_module));
		asn1p_module_free((yyvsp[0].a_module));
	}
#line 3458 "asn1p_y.c"
    break;

  case 31: /* Assignment: DataTypeReference  */
#line 727 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		assert((yyvsp[0].a_expr)->expr_type != A1TC_INVALID);
		assert((yyvsp[0].a_expr)->meta_type != AMT_INVALID);
		asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
	}
#line 3470 "asn1p_y.c"
    break;

  case 32: /* Assignment: ValueAssignment  */
#line 734 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		assert((yyvsp[0].a_expr)->expr_type != A1TC_INVALID);
		assert((yyvsp[0].a_expr)->meta_type != AMT_INVALID);
		asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
	}
#line 3482 "asn1p_y.c"
    break;

  case 33: /* Assignment: ValueSetTypeAssignment  */
#line 748 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                 {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		assert((yyvsp[0].a_expr)->expr_type != A1TC_INVALID);
		assert((yyvsp[0].a_expr)->meta_type != AMT_INVALID);
		asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
	}
#line 3494 "asn1p_y.c"
    break;

  case 34: /* $@2: %empty  */
#line 756 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                { asn1p_lexer_hack_push_encoding_control(); }
#line 3500 "asn1p_y.c"
    break;

  case 35: /* Assignment: TOK_ENCODING_CONTROL TOK_capitalreference $@2 EncodingControlBody TOK_END  */
#line 758 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {
		(yyval.a_module) = asn1p_module_new();
		if((yyval.a_module)) {
			/* Store encoding instructions as module members */
			if((yyvsp[-1].a_module)) {
				asn1p_expr_t *instr;
				int count = 0;
				TQ_FOR(instr, &((yyvsp[-1].a_module)->members), next) {
					asn1p_expr_t *copy = asn1p_expr_clone(instr, 0);
					if(copy) {
						free(copy->encoding_control.encoding_reference);
						copy->encoding_control.encoding_reference = strdup((yyvsp[-3].tv_str));
						asn1p_module_member_add((yyval.a_module), copy);
						count++;
					}
				}
				asn1p_module_free((yyvsp[-1].a_module));
				
				fprintf(stderr,
					"NOTE: ENCODING-CONTROL %s at %s:%d with %d directive(s)\n",
					(yyvsp[-3].tv_str), ASN_FILENAME, yylineno, count);
			} else {
				fprintf(stderr,
					"NOTE: ENCODING-CONTROL %s at %s:%d with 0 directive(s)\n",
					(yyvsp[-3].tv_str), ASN_FILENAME, yylineno);
			}
			free((yyvsp[-3].tv_str));
		} else {
			free((yyvsp[-3].tv_str));
		}
	}
#line 3536 "asn1p_y.c"
    break;

  case 36: /* EncodingControlBody: %empty  */
#line 792 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_module) = asn1p_module_new(); }
#line 3542 "asn1p_y.c"
    break;

  case 37: /* EncodingControlBody: EncodingInstructionList  */
#line 794 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_module) = (yyvsp[0].a_module); }
#line 3548 "asn1p_y.c"
    break;

  case 38: /* EncodingInstructionList: EncodingInstruction  */
#line 799 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		if((yyvsp[0].a_expr)) {
			asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
		}
	}
#line 3560 "asn1p_y.c"
    break;

  case 39: /* EncodingInstructionList: EncodingInstructionList EncodingInstruction  */
#line 807 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		(yyval.a_module) = (yyvsp[-1].a_module);
		if((yyvsp[0].a_expr)) {
			asn1p_module_member_add((yyval.a_module), (yyvsp[0].a_expr));
		}
	}
#line 3571 "asn1p_y.c"
    break;

  case 40: /* EncodingInstruction: TOK_typereference TOK_OCTET TOK_STRING TOK_PPEQ Identifier  */
#line 817 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		/* TypeName OCTET STRING ::= hexadecimal */
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		
		(yyval.a_expr)->Identifier = (yyvsp[-4].tv_str);  /* Type name */
		(yyval.a_expr)->meta_type = AMT_TYPE;
		(yyval.a_expr)->expr_type = ASN_BASIC_OCTET_STRING;
		(yyval.a_expr)->_mark = TM_ENCODING_INSTRUCTION;  /* Mark as encoding instruction */
		
		/* Parse encoding format */
		if(strcmp((yyvsp[0].tv_str), "hexadecimal") == 0) {
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_HEXADECIMAL;
		} else if(strcmp((yyvsp[0].tv_str), "base64") == 0) {
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_BASE64;
		} else if(strcmp((yyvsp[0].tv_str), "utf8") == 0) {
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_UTF8;
		} else {
			fprintf(stderr,
				"WARNING: Unknown encoding format '%s' at %s:%d\n",
				(yyvsp[0].tv_str), ASN_FILENAME, yylineno);
			(yyval.a_expr)->encoding_control.encoding_type = EC_NONE;
		}
		
		(yyval.a_expr)->encoding_control.encoding_reference = strdup("XER");
		
		free((yyvsp[0].tv_str));
	}
#line 3604 "asn1p_y.c"
    break;

  case 41: /* EncodingInstruction: Identifier TOK_OCTET TOK_STRING TOK_PPEQ Identifier  */
#line 846 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		/* fieldName OCTET STRING ::= hexadecimal */
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		
		(yyval.a_expr)->Identifier = (yyvsp[-4].tv_str);  /* Field name */
		(yyval.a_expr)->meta_type = AMT_TYPE;
		(yyval.a_expr)->expr_type = ASN_BASIC_OCTET_STRING;
		(yyval.a_expr)->_mark = TM_ENCODING_INSTRUCTION;  /* Mark as encoding instruction */
		
		/* Parse encoding format */
		if(strcmp((yyvsp[0].tv_str), "hexadecimal") == 0) {
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_HEXADECIMAL;
		} else if(strcmp((yyvsp[0].tv_str), "base64") == 0) {
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_BASE64;
		} else if(strcmp((yyvsp[0].tv_str), "utf8") == 0) {
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_UTF8;
		} else {
			fprintf(stderr,
				"WARNING: Unknown encoding format '%s' at %s:%d\n",
				(yyvsp[0].tv_str), ASN_FILENAME, yylineno);
			(yyval.a_expr)->encoding_control.encoding_type = EC_NONE;
		}

		if((yyval.a_expr)->encoding_control.encoding_reference != NULL) {
			free((yyval.a_expr)->encoding_control.encoding_reference);
			(yyval.a_expr)->encoding_control.encoding_reference = NULL;
		}
		(yyval.a_expr)->encoding_control.encoding_reference = strdup("XER");
		if((yyval.a_expr)->encoding_control.encoding_reference == NULL) {
			fprintf(stderr,
				"ERROR: Memory allocation failed for encoding_reference at %s:%d\n",
				ASN_FILENAME, yylineno);
			YYABORT;
		}
		
		free((yyvsp[0].tv_str));
	}
#line 3647 "asn1p_y.c"
    break;

  case 42: /* EncodingInstruction: TOK_typereference TOK_typereference  */
#line 890 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		(yyval.a_expr) = NULL;
		if(strcmp((yyvsp[-1].tv_str), "BASE64") == 0) {
			/* BASE64 TypeName */
			(yyval.a_expr) = NEW_EXPR();
			checkmem((yyval.a_expr));
			(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
			(yyval.a_expr)->meta_type = AMT_TYPE;
			(yyval.a_expr)->expr_type = ASN_BASIC_OCTET_STRING;
			(yyval.a_expr)->_mark = TM_ENCODING_INSTRUCTION;
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_BASE64;
			(yyval.a_expr)->encoding_control.encoding_reference = strdup("XER");
			free((yyvsp[-1].tv_str));
		} else if(strcmp((yyvsp[-1].tv_str), "DECIMAL") == 0) {
			(yyval.a_expr) = NEW_EXPR();
			checkmem((yyval.a_expr));
			(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
			(yyval.a_expr)->meta_type = AMT_TYPE;
			(yyval.a_expr)->expr_type = ASN_BASIC_REAL;
			(yyval.a_expr)->_mark = TM_ENCODING_INSTRUCTION;
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_DECIMAL;
			(yyval.a_expr)->encoding_control.encoding_reference = strdup("XER");
			free((yyvsp[-1].tv_str));
		} else if(strcmp((yyvsp[-1].tv_str), "TEXT") == 0) {
			(yyval.a_expr) = NEW_EXPR();
			checkmem((yyval.a_expr));
			(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
			(yyval.a_expr)->meta_type = AMT_TYPE;
			(yyval.a_expr)->expr_type = A1TC_REFERENCE;
			(yyval.a_expr)->_mark = TM_ENCODING_INSTRUCTION;
			(yyval.a_expr)->encoding_control.encoding_type = EC_XER_TEXT;
			(yyval.a_expr)->encoding_control.encoding_reference = strdup("XER");
			free((yyvsp[-1].tv_str));
		} else if(strcmp((yyvsp[-1].tv_str), "GLOBAL-DEFAULTS") == 0
		       && strcmp((yyvsp[0].tv_str), "MODIFIED-ENCODINGS") == 0) {
			(yyval.a_expr) = NEW_EXPR();
			checkmem((yyval.a_expr));
			(yyval.a_expr)->Identifier = strdup("__GLOBAL_DEFAULTS_MODIFIED_ENCODINGS__");
			checkmem((yyval.a_expr)->Identifier);
			(yyval.a_expr)->meta_type = AMT_TYPE;
			(yyval.a_expr)->expr_type = A1TC_REFERENCE;
			(yyval.a_expr)->_mark = TM_ENCODING_INSTRUCTION;
			(yyval.a_expr)->encoding_control.encoding_type =
				EC_XER_GLOBAL_DEFAULTS_MODIFIED_ENCODINGS;
			(yyval.a_expr)->encoding_control.encoding_reference = strdup("XER");
			free((yyvsp[-1].tv_str));
			free((yyvsp[0].tv_str));
		} else {
			fprintf(stderr,
				"WARNING: Unknown ENCODING-CONTROL directive '%s %s' at %s:%d\n",
				(yyvsp[-1].tv_str), (yyvsp[0].tv_str), ASN_FILENAME, yylineno);
			free((yyvsp[-1].tv_str));
			free((yyvsp[0].tv_str));
		}
	}
#line 3707 "asn1p_y.c"
    break;

  case 43: /* EncodingInstruction: TOK_typereference EncodingControlTarget EncodingControlOptAs  */
#line 946 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		(yyval.a_expr) = NULL;
		if(strcmp((yyvsp[-2].tv_str), "BASE64") == 0
		|| strcmp((yyvsp[-2].tv_str), "DECIMAL") == 0
		|| strcmp((yyvsp[-2].tv_str), "TEXT") == 0
		|| strcmp((yyvsp[-2].tv_str), "NAME") == 0) {
			(yyval.a_expr) = NEW_EXPR();
			checkmem((yyval.a_expr));
			(yyval.a_expr)->Identifier = strdup((yyvsp[-1].tv_str));
			checkmem((yyval.a_expr)->Identifier);
			(yyval.a_expr)->meta_type = AMT_TYPE;
			(yyval.a_expr)->expr_type = A1TC_REFERENCE;
			(yyval.a_expr)->_mark = TM_ENCODING_INSTRUCTION;
			(yyval.a_expr)->encoding_control.target_path = (yyvsp[-1].tv_str);
			if((yyvsp[0].tv_str)) (yyval.a_expr)->encoding_control.replacement = (yyvsp[0].tv_str);
			if(strcmp((yyvsp[-2].tv_str), "BASE64") == 0) {
				(yyval.a_expr)->encoding_control.encoding_type = EC_XER_BASE64;
			} else if(strcmp((yyvsp[-2].tv_str), "DECIMAL") == 0) {
				(yyval.a_expr)->encoding_control.encoding_type = EC_XER_DECIMAL;
			} else if(strcmp((yyvsp[-2].tv_str), "TEXT") == 0) {
				(yyval.a_expr)->encoding_control.encoding_type = EC_XER_TEXT;
			} else {
				(yyval.a_expr)->encoding_control.encoding_type = EC_JER_NAME;
			}
			free((yyvsp[-2].tv_str));
		} else {
			fprintf(stderr,
				"WARNING: Unknown ENCODING-CONTROL directive '%s %s' at %s:%d\n",
				(yyvsp[-2].tv_str), (yyvsp[-1].tv_str), ASN_FILENAME, yylineno);
			free((yyvsp[-2].tv_str));
			free((yyvsp[-1].tv_str));
			free((yyvsp[0].tv_str));
		}
	}
#line 3746 "asn1p_y.c"
    break;

  case 44: /* EncodingInstruction: error  */
#line 981 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		/* Error recovery - skip malformed instruction */
		fprintf(stderr,
			"WARNING: Malformed encoding instruction at %s:%d\n",
			ASN_FILENAME, yylineno);
		(yyval.a_expr) = NULL;
	}
#line 3758 "asn1p_y.c"
    break;

  case 46: /* EncodingControlTarget: EncodingControlTarget '.' EncodingControlTargetAtom  */
#line 992 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                              {
		(yyval.tv_str) = _encoding_control_join((yyvsp[-2].tv_str), (yyvsp[0].tv_str));
		checkmem((yyval.tv_str));
	}
#line 3767 "asn1p_y.c"
    break;

  case 47: /* EncodingControlTargetAtom: TOK_typereference  */
#line 999 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          { (yyval.tv_str) = (yyvsp[0].tv_str); }
#line 3773 "asn1p_y.c"
    break;

  case 48: /* EncodingControlTargetAtom: "identifier"  */
#line 1000 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                         { (yyval.tv_str) = (yyvsp[0].tv_str); }
#line 3779 "asn1p_y.c"
    break;

  case 49: /* EncodingControlTargetAtom: TOK_capitalreference  */
#line 1001 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               { (yyval.tv_str) = (yyvsp[0].tv_str); }
#line 3785 "asn1p_y.c"
    break;

  case 50: /* EncodingControlOptAs: %empty  */
#line 1005 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.tv_str) = NULL; }
#line 3791 "asn1p_y.c"
    break;

  case 51: /* EncodingControlOptAs: "identifier" EncodingControlAsValue  */
#line 1006 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                {
		if(strcmp((yyvsp[-1].tv_str), "AS") == 0) {
			(yyval.tv_str) = (yyvsp[0].tv_str);
		} else {
			fprintf(stderr,
				"WARNING: Unknown ENCODING-CONTROL token '%s' at %s:%d\n",
				(yyvsp[-1].tv_str), ASN_FILENAME, yylineno);
			free((yyvsp[0].tv_str));
			(yyval.tv_str) = NULL;
		}
		free((yyvsp[-1].tv_str));
	}
#line 3808 "asn1p_y.c"
    break;

  case 52: /* EncodingControlOptAs: TOK_typereference EncodingControlAsValue  */
#line 1018 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                   {
		if(strcmp((yyvsp[-1].tv_str), "AS") == 0) {
			(yyval.tv_str) = (yyvsp[0].tv_str);
		} else {
			fprintf(stderr,
				"WARNING: Unknown ENCODING-CONTROL token '%s' at %s:%d\n",
				(yyvsp[-1].tv_str), ASN_FILENAME, yylineno);
			free((yyvsp[0].tv_str));
			(yyval.tv_str) = NULL;
		}
		free((yyvsp[-1].tv_str));
	}
#line 3825 "asn1p_y.c"
    break;

  case 53: /* EncodingControlAsValue: TOK_typereference  */
#line 1033 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          { (yyval.tv_str) = (yyvsp[0].tv_str); }
#line 3831 "asn1p_y.c"
    break;

  case 54: /* EncodingControlAsValue: "identifier"  */
#line 1034 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                         { (yyval.tv_str) = (yyvsp[0].tv_str); }
#line 3837 "asn1p_y.c"
    break;

  case 55: /* EncodingControlAsValue: TOK_capitalreference  */
#line 1035 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               { (yyval.tv_str) = (yyvsp[0].tv_str); }
#line 3843 "asn1p_y.c"
    break;

  case 56: /* EncodingControlAsValue: TOK_cstring  */
#line 1036 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      { (yyval.tv_str) = _encoding_control_cstring((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len); checkmem((yyval.tv_str)); }
#line 3849 "asn1p_y.c"
    break;

  case 57: /* EncodingControlAsValue: BasicString  */
#line 1042 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.tv_str) = NULL;
		return yyerror(param,
			"Attempt to redefine a standard basic string type, "
			"please comment out or remove this type redefinition.");
	}
#line 3860 "asn1p_y.c"
    break;

  case 58: /* optImports: %empty  */
#line 1056 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_module) = 0; }
#line 3866 "asn1p_y.c"
    break;

  case 60: /* ImportsDefinition: TOK_IMPORTS optImportsBundleSet ';'  */
#line 1060 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                            {
		if(!saved_aid && 0)
			return yyerror(param, "Unterminated IMPORTS FROM, "
					"expected semicolon ';'");
		saved_aid = 0;
		(yyval.a_module) = (yyvsp[-1].a_module);
	}
#line 3878 "asn1p_y.c"
    break;

  case 61: /* ImportsDefinition: TOK_IMPORTS TOK_FROM  */
#line 1070 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_module) = NULL;
		return yyerror(param, "Empty IMPORTS list");
	}
#line 3887 "asn1p_y.c"
    break;

  case 62: /* optImportsBundleSet: %empty  */
#line 1077 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_module) = asn1p_module_new(); }
#line 3893 "asn1p_y.c"
    break;

  case 64: /* ImportsBundleSet: ImportsBundle  */
#line 1081 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		TQ_ADD(&((yyval.a_module)->imports), (yyvsp[0].a_xports), xp_next);
	}
#line 3903 "asn1p_y.c"
    break;

  case 65: /* ImportsBundleSet: ImportsBundleSet ImportsBundle  */
#line 1086 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_module) = (yyvsp[-1].a_module);
		TQ_ADD(&((yyval.a_module)->imports), (yyvsp[0].a_xports), xp_next);
	}
#line 3912 "asn1p_y.c"
    break;

  case 66: /* AssignedIdentifier: %empty  */
#line 1093 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { memset(&(yyval.a_aid), 0, sizeof((yyval.a_aid))); }
#line 3918 "asn1p_y.c"
    break;

  case 67: /* AssignedIdentifier: ObjectIdentifier  */
#line 1094 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                           { (yyval.a_aid).oid = (yyvsp[0].a_oid); }
#line 3924 "asn1p_y.c"
    break;

  case 68: /* ImportsBundle: ImportsBundleInt ImportSelectionOption  */
#line 1098 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                           {
      (yyval.a_xports) = (yyvsp[-1].a_xports);
      (yyval.a_xports)->option = (yyvsp[0].a_int);
    }
#line 3933 "asn1p_y.c"
    break;

  case 70: /* ImportsBundleInt: ImportsList TOK_FROM TypeRefName AssignedIdentifier  */
#line 1105 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                            {
		(yyval.a_xports) = (yyvsp[-3].a_xports);
		(yyval.a_xports)->fromModuleName = (yyvsp[-1].tv_str);
		(yyval.a_xports)->identifier = (yyvsp[0].a_aid);
		(yyval.a_xports)->option = 0;
		/* This stupid thing is used for look-back hack. */
		saved_aid = (yyval.a_xports)->identifier.oid ? 0 : &((yyval.a_xports)->identifier);
		checkmem((yyval.a_xports));
	}
#line 3947 "asn1p_y.c"
    break;

  case 71: /* ImportsList: ImportsElement  */
#line 1117 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_xports) = asn1p_xports_new();
		checkmem((yyval.a_xports));
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 3957 "asn1p_y.c"
    break;

  case 72: /* ImportsList: ImportsList ',' ImportsElement  */
#line 1122 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_xports) = (yyvsp[-2].a_xports);
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 3966 "asn1p_y.c"
    break;

  case 73: /* ImportsElement: TypeRefName  */
#line 1129 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
	}
#line 3977 "asn1p_y.c"
    break;

  case 74: /* ImportsElement: TypeRefName '{' '}'  */
#line 1135 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              {		/* Completely equivalent to above */
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
	}
#line 3988 "asn1p_y.c"
    break;

  case 75: /* ImportsElement: Identifier  */
#line 1141 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
	}
#line 3999 "asn1p_y.c"
    break;

  case 76: /* ImportSelectionOption: TOK_WITH TOK_SUCCESSORS  */
#line 1150 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                {
		(yyval.a_int) = XPT_WITH_SUCCESSORS;
	}
#line 4007 "asn1p_y.c"
    break;

  case 77: /* ImportSelectionOption: TOK_WITH TOK_DESCENDANTS  */
#line 1153 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                   {
		(yyval.a_int) = XPT_WITH_DESCENDANTS;
	}
#line 4015 "asn1p_y.c"
    break;

  case 78: /* optExports: %empty  */
#line 1159 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_module) = 0; }
#line 4021 "asn1p_y.c"
    break;

  case 79: /* optExports: ExportsDefinition  */
#line 1160 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		(yyval.a_module) = asn1p_module_new();
		checkmem((yyval.a_module));
		if((yyvsp[0].a_xports)) {
			TQ_ADD(&((yyval.a_module)->exports), (yyvsp[0].a_xports), xp_next);
		} else {
			/* "EXPORTS ALL;" */
		}
	}
#line 4035 "asn1p_y.c"
    break;

  case 80: /* ExportsDefinition: TOK_EXPORTS ExportsBody ';'  */
#line 1172 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                    {
		(yyval.a_xports) = (yyvsp[-1].a_xports);
	}
#line 4043 "asn1p_y.c"
    break;

  case 81: /* ExportsDefinition: TOK_EXPORTS TOK_ALL ';'  */
#line 1175 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  {
		(yyval.a_xports) = 0;
	}
#line 4051 "asn1p_y.c"
    break;

  case 82: /* ExportsDefinition: TOK_EXPORTS ';'  */
#line 1178 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		/* Empty EXPORTS clause effectively prohibits export. */
		(yyval.a_xports) = asn1p_xports_new();
		checkmem((yyval.a_xports));
	}
#line 4061 "asn1p_y.c"
    break;

  case 83: /* ExportsBody: ExportsElement  */
#line 1186 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_xports) = asn1p_xports_new();
		assert((yyval.a_xports));
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 4071 "asn1p_y.c"
    break;

  case 84: /* ExportsBody: ExportsBody ',' ExportsElement  */
#line 1191 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_xports) = (yyvsp[-2].a_xports);
		TQ_ADD(&((yyval.a_xports)->xp_members), (yyvsp[0].a_expr), next);
	}
#line 4080 "asn1p_y.c"
    break;

  case 85: /* ExportsElement: TypeRefName  */
#line 1198 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_EXPORTVAR;
	}
#line 4091 "asn1p_y.c"
    break;

  case 86: /* ExportsElement: TypeRefName '{' '}'  */
#line 1204 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->expr_type = A1TC_EXPORTVAR;
	}
#line 4102 "asn1p_y.c"
    break;

  case 87: /* ExportsElement: Identifier  */
#line 1210 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
		(yyval.a_expr)->expr_type = A1TC_EXPORTVAR;
	}
#line 4113 "asn1p_y.c"
    break;

  case 88: /* ValueSet: '{' ElementSetSpecs '}'  */
#line 1219 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  { (yyval.a_constr) = (yyvsp[-1].a_constr); }
#line 4119 "asn1p_y.c"
    break;

  case 89: /* ValueSetTypeAssignment: TypeRefName Type TOK_PPEQ ValueSet  */
#line 1222 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		assert((yyval.a_expr)->Identifier == 0);
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->meta_type = AMT_VALUESET;
		(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
	}
#line 4131 "asn1p_y.c"
    break;

  case 90: /* DefinedType: ComplexTypeReference  */
#line 1242 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = (yyvsp[0].a_ref);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_TYPEREF;
	}
#line 4143 "asn1p_y.c"
    break;

  case 91: /* DefinedType: ComplexTypeReference '{' ActualParameterList '}'  */
#line 1252 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                           {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = (yyvsp[-3].a_ref);
		(yyval.a_expr)->rhs_pspecs = (yyvsp[-1].a_expr);
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_TYPEREF;
	}
#line 4156 "asn1p_y.c"
    break;

  case 92: /* DataTypeReference: TypeRefName TOK_PPEQ Type  */
#line 1272 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		assert((yyval.a_expr)->expr_type);
		assert((yyval.a_expr)->meta_type);
	}
#line 4167 "asn1p_y.c"
    break;

  case 93: /* DataTypeReference: TypeRefName TOK_PPEQ ObjectClass  */
#line 1278 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		assert((yyval.a_expr)->expr_type == A1TC_CLASSDEF);
		assert((yyval.a_expr)->meta_type == AMT_OBJECTCLASS);
	}
#line 4178 "asn1p_y.c"
    break;

  case 94: /* DataTypeReference: TypeRefName '{' ParameterArgumentList '}' TOK_PPEQ Type  */
#line 1294 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                  {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-5].tv_str);
		(yyval.a_expr)->lhs_params = (yyvsp[-3].a_plist);
	}
#line 4188 "asn1p_y.c"
    break;

  case 95: /* DataTypeReference: TypeRefName '{' ParameterArgumentList '}' TOK_PPEQ ObjectClass  */
#line 1300 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                         {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->Identifier = (yyvsp[-5].tv_str);
		(yyval.a_expr)->lhs_params = (yyvsp[-3].a_plist);
	}
#line 4198 "asn1p_y.c"
    break;

  case 96: /* ParameterArgumentList: ParameterArgumentName  */
#line 1308 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              {
		int ret;
		(yyval.a_plist) = asn1p_paramlist_new(yylineno);
		checkmem((yyval.a_plist));
		ret = asn1p_paramlist_add_param((yyval.a_plist), (yyvsp[0].a_parg).governor, (yyvsp[0].a_parg).argument);
		checkmem(ret == 0);
		asn1p_ref_free((yyvsp[0].a_parg).governor);
		free((yyvsp[0].a_parg).argument);
	}
#line 4212 "asn1p_y.c"
    break;

  case 97: /* ParameterArgumentList: ParameterArgumentList ',' ParameterArgumentName  */
#line 1317 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                          {
		int ret;
		(yyval.a_plist) = (yyvsp[-2].a_plist);
		ret = asn1p_paramlist_add_param((yyval.a_plist), (yyvsp[0].a_parg).governor, (yyvsp[0].a_parg).argument);
		checkmem(ret == 0);
		asn1p_ref_free((yyvsp[0].a_parg).governor);
		free((yyvsp[0].a_parg).argument);
	}
#line 4225 "asn1p_y.c"
    break;

  case 98: /* ParameterArgumentName: TypeRefName  */
#line 1328 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_parg).governor = NULL;
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
	}
#line 4234 "asn1p_y.c"
    break;

  case 99: /* ParameterArgumentName: TypeRefName ':' Identifier  */
#line 1332 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                     {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(yylineno, currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor, (yyvsp[-2].tv_str), 0);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
		free((yyvsp[-2].tv_str));
	}
#line 4247 "asn1p_y.c"
    break;

  case 100: /* ParameterArgumentName: TypeRefName ':' TypeRefName  */
#line 1340 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(yylineno, currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor, (yyvsp[-2].tv_str), 0);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
		free((yyvsp[-2].tv_str));
	}
#line 4260 "asn1p_y.c"
    break;

  case 101: /* ParameterArgumentName: BasicTypeId ':' Identifier  */
#line 1348 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                     {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(yylineno, currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor,
			ASN_EXPR_TYPE2STR((yyvsp[-2].a_type)), 1);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
	}
#line 4273 "asn1p_y.c"
    break;

  case 102: /* ParameterArgumentName: BasicTypeId ':' TypeRefName  */
#line 1356 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
		int ret;
		(yyval.a_parg).governor = asn1p_ref_new(yylineno, currentModule);
		ret = asn1p_ref_add_component((yyval.a_parg).governor,
			ASN_EXPR_TYPE2STR((yyvsp[-2].a_type)), 1);
		checkmem(ret == 0);
		(yyval.a_parg).argument = (yyvsp[0].tv_str);
	}
#line 4286 "asn1p_y.c"
    break;

  case 103: /* ActualParameterList: ActualParameter  */
#line 1367 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4296 "asn1p_y.c"
    break;

  case 104: /* ActualParameterList: ActualParameterList ',' ActualParameter  */
#line 1372 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                  {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4305 "asn1p_y.c"
    break;

  case 106: /* ActualParameter: SimpleValue  */
#line 1380 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("?");
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 4318 "asn1p_y.c"
    break;

  case 107: /* ActualParameter: DefinedValue  */
#line 1388 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("?");
		(yyval.a_expr)->expr_type = A1TC_REFERENCE;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 4331 "asn1p_y.c"
    break;

  case 108: /* ActualParameter: ValueSet  */
#line 1396 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->expr_type = A1TC_VALUESET;
		(yyval.a_expr)->meta_type = AMT_VALUESET;
		(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
	}
#line 4342 "asn1p_y.c"
    break;

  case 109: /* optComponentTypeLists: %empty  */
#line 1419 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_expr) = NEW_EXPR(); }
#line 4348 "asn1p_y.c"
    break;

  case 110: /* optComponentTypeLists: ComponentTypeLists  */
#line 1420 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             { (yyval.a_expr) = (yyvsp[0].a_expr); }
#line 4354 "asn1p_y.c"
    break;

  case 111: /* ComponentTypeLists: ComponentType  */
#line 1423 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4364 "asn1p_y.c"
    break;

  case 112: /* ComponentTypeLists: ComponentTypeLists ',' ComponentType  */
#line 1428 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                               {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4373 "asn1p_y.c"
    break;

  case 113: /* ComponentTypeLists: ComponentTypeLists ',' TOK_VBracketLeft ComponentTypeLists TOK_VBracketRight  */
#line 1432 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                                       {
		(yyval.a_expr) = (yyvsp[-4].a_expr);
		(yyvsp[-1].a_expr)->meta_type = AMT_TYPE;
		(yyvsp[-1].a_expr)->expr_type = ASN_CONSTR_SEQUENCE;
		(yyvsp[-1].a_expr)->marker.flags |= EM_OPTIONAL | EM_EXTENSION_GROUP;
		asn1p_expr_add((yyval.a_expr), (yyvsp[-1].a_expr));
	}
#line 4385 "asn1p_y.c"
    break;

  case 114: /* ComponentTypeLists: ComponentTypeLists TOK_VBracketLeft ComponentTypeLists TOK_VBracketRight  */
#line 1439 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                                   {
		(yyval.a_expr) = (yyvsp[-3].a_expr);
		(yyvsp[-1].a_expr)->meta_type = AMT_TYPE;
		(yyvsp[-1].a_expr)->expr_type = ASN_CONSTR_SEQUENCE;
		(yyvsp[-1].a_expr)->marker.flags |= EM_OPTIONAL | EM_EXTENSION_GROUP;
		asn1p_expr_add((yyval.a_expr), (yyvsp[-1].a_expr));
	}
#line 4397 "asn1p_y.c"
    break;

  case 115: /* ComponentType: Identifier MaybeIndirectTaggedType optMarker  */
#line 1449 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                     {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->Identifier == 0);
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyvsp[0].a_marker).flags |= (yyval.a_expr)->marker.flags;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 4409 "asn1p_y.c"
    break;

  case 116: /* ComponentType: MaybeIndirectTaggedType optMarker  */
#line 1456 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                            {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		(yyvsp[0].a_marker).flags |= (yyval.a_expr)->marker.flags;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
		_fixup_anonymous_identifier((yyval.a_expr));
	}
#line 4420 "asn1p_y.c"
    break;

  case 117: /* ComponentType: TOK_COMPONENTS TOK_OF MaybeIndirectTaggedType  */
#line 1462 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->meta_type = (yyvsp[0].a_expr)->meta_type;
		(yyval.a_expr)->expr_type = A1TC_COMPONENTS_OF;
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4432 "asn1p_y.c"
    break;

  case 118: /* ComponentType: ExtensionAndException  */
#line 1469 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                {
		(yyval.a_expr) = (yyvsp[0].a_expr);
	}
#line 4440 "asn1p_y.c"
    break;

  case 119: /* AlternativeTypeLists: AlternativeType  */
#line 1475 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4450 "asn1p_y.c"
    break;

  case 120: /* AlternativeTypeLists: AlternativeTypeLists ',' AlternativeType  */
#line 1480 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                   {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4459 "asn1p_y.c"
    break;

  case 121: /* AlternativeTypeLists: AlternativeTypeLists ',' TOK_VBracketLeft AlternativeTypeLists TOK_VBracketRight  */
#line 1484 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                                           {
		(yyval.a_expr) = (yyvsp[-4].a_expr);
		asn1p_expr_add_many((yyval.a_expr), (yyvsp[-1].a_expr));
		asn1p_expr_free((yyvsp[-1].a_expr));
	}
#line 4469 "asn1p_y.c"
    break;

  case 122: /* AlternativeTypeLists: AlternativeTypeLists TOK_VBracketLeft AlternativeTypeLists TOK_VBracketRight  */
#line 1489 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                                       {
		(yyval.a_expr) = (yyvsp[-3].a_expr);
		asn1p_expr_add_many((yyval.a_expr), (yyvsp[-1].a_expr));
		asn1p_expr_free((yyvsp[-1].a_expr));
	}
#line 4479 "asn1p_y.c"
    break;

  case 123: /* AlternativeType: Identifier MaybeIndirectTaggedType  */
#line 1497 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		assert((yyval.a_expr)->Identifier == 0);
		(yyval.a_expr)->Identifier = (yyvsp[-1].tv_str);
	}
#line 4489 "asn1p_y.c"
    break;

  case 124: /* AlternativeType: ExtensionAndException  */
#line 1502 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                {
		(yyval.a_expr) = (yyvsp[0].a_expr);
	}
#line 4497 "asn1p_y.c"
    break;

  case 125: /* AlternativeType: MaybeIndirectTaggedType  */
#line 1505 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  {
		(yyval.a_expr) = (yyvsp[0].a_expr);
		_fixup_anonymous_identifier((yyval.a_expr));
	}
#line 4506 "asn1p_y.c"
    break;

  case 126: /* ObjectClass: TOK_CLASS '{' FieldSpec '}' optWithSyntax  */
#line 1512 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                  {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		checkmem((yyval.a_expr));
		(yyval.a_expr)->with_syntax = (yyvsp[0].a_wsynt);
		assert((yyval.a_expr)->expr_type == A1TC_CLASSDEF);
		assert((yyval.a_expr)->meta_type == AMT_OBJECTCLASS);
	}
#line 4518 "asn1p_y.c"
    break;

  case 127: /* optUNIQUE: %empty  */
#line 1522 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_int) = 0; }
#line 4524 "asn1p_y.c"
    break;

  case 128: /* optUNIQUE: TOK_UNIQUE  */
#line 1523 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     { (yyval.a_int) = 1; }
#line 4530 "asn1p_y.c"
    break;

  case 129: /* FieldSpec: ClassField  */
#line 1527 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_CLASSDEF;
		(yyval.a_expr)->meta_type = AMT_OBJECTCLASS;
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4542 "asn1p_y.c"
    break;

  case 130: /* FieldSpec: FieldSpec ',' ClassField  */
#line 1534 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                   {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4551 "asn1p_y.c"
    break;

  case 131: /* ClassField: TOK_typefieldreference optMarker  */
#line 1544 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-1].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_TFS;	/* TypeFieldSpec */
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 4564 "asn1p_y.c"
    break;

  case 132: /* ClassField: TOK_valuefieldreference Type optUNIQUE optMarker  */
#line 1554 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                           {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_FTVFS;	/* FixedTypeValueFieldSpec */
		(yyval.a_expr)->unique = (yyvsp[-1].a_int);
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
		asn1p_expr_add((yyval.a_expr), (yyvsp[-2].a_expr));
	}
#line 4578 "asn1p_y.c"
    break;

  case 133: /* ClassField: TOK_valuefieldreference FieldName optMarker  */
#line 1565 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                      {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_VTVFS;
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 4591 "asn1p_y.c"
    break;

  case 134: /* ClassField: TOK_valuefieldreference DefinedObjectClass optMarker  */
#line 1575 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                               {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_OFS;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 4605 "asn1p_y.c"
    break;

  case 135: /* ClassField: TOK_typefieldreference FieldName optMarker  */
#line 1586 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                     {
		(yyval.a_expr) = NEW_EXPR();
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_VTVSFS;
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 4618 "asn1p_y.c"
    break;

  case 136: /* ClassField: TOK_typefieldreference Type optMarker  */
#line 1596 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_FTVSFS;
		asn1p_expr_add((yyval.a_expr), (yyvsp[-1].a_expr));
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 4632 "asn1p_y.c"
    break;

  case 137: /* ClassField: TOK_typefieldreference DefinedObjectClass optMarker  */
#line 1607 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                              {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyval.a_expr)->reference = (yyvsp[-1].a_ref);
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_OSFS;
		(yyval.a_expr)->marker = (yyvsp[0].a_marker);
	}
#line 4646 "asn1p_y.c"
    break;

  case 138: /* ClassField: TOK_IDENTIFIED TOK_BY PrimitiveFieldReference  */
#line 1618 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("IDENTIFIED-BY");
		(yyval.a_expr)->meta_type = AMT_OBJECTFIELD;
		(yyval.a_expr)->expr_type = A1TC_CLASSFIELD_FTVFS;  /* Treat as fixed type value field */
		(yyval.a_expr)->reference = asn1p_ref_new(yylineno, currentModule);
		asn1p_ref_add_component((yyval.a_expr)->reference, (yyvsp[0].a_refcomp).name, (yyvsp[0].a_refcomp).lex_type);
		free((yyvsp[0].a_refcomp).name);
	}
#line 4661 "asn1p_y.c"
    break;

  case 139: /* optWithSyntax: %empty  */
#line 1631 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_wsynt) = 0; }
#line 4667 "asn1p_y.c"
    break;

  case 140: /* optWithSyntax: WithSyntax  */
#line 1632 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.a_wsynt) = (yyvsp[0].a_wsynt);
	}
#line 4675 "asn1p_y.c"
    break;

  case 141: /* $@3: %empty  */
#line 1639 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                { asn1p_lexer_hack_enable_with_syntax(); }
#line 4681 "asn1p_y.c"
    break;

  case 142: /* WithSyntax: TOK_WITH TOK_SYNTAX '{' $@3 WithSyntaxList '}'  */
#line 1641 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_wsynt) = (yyvsp[-1].a_wsynt);
	}
#line 4689 "asn1p_y.c"
    break;

  case 143: /* WithSyntaxList: WithSyntaxToken  */
#line 1647 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {
		(yyval.a_wsynt) = asn1p_wsyntx_new();
		TQ_ADD(&((yyval.a_wsynt)->chunks), (yyvsp[0].a_wchunk), next);
	}
#line 4698 "asn1p_y.c"
    break;

  case 144: /* WithSyntaxList: WithSyntaxList WithSyntaxToken  */
#line 1651 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_wsynt) = (yyvsp[-1].a_wsynt);
		TQ_ADD(&((yyval.a_wsynt)->chunks), (yyvsp[0].a_wchunk), next);
	}
#line 4707 "asn1p_y.c"
    break;

  case 145: /* WithSyntaxToken: TOK_whitespace  */
#line 1658 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromstring((yyvsp[0].tv_opaque).buf, 0);
		(yyval.a_wchunk)->type = WC_WHITESPACE;
	}
#line 4716 "asn1p_y.c"
    break;

  case 146: /* WithSyntaxToken: TOK_Literal  */
#line 1662 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromstring((yyvsp[0].tv_str), 0);
	}
#line 4724 "asn1p_y.c"
    break;

  case 147: /* WithSyntaxToken: PrimitiveFieldReference  */
#line 1665 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromstring((yyvsp[0].a_refcomp).name, 0);
		(yyval.a_wchunk)->type = WC_FIELD;
	}
#line 4733 "asn1p_y.c"
    break;

  case 148: /* WithSyntaxToken: '[' WithSyntaxList ']'  */
#line 1669 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                 {
		(yyval.a_wchunk) = asn1p_wsyntx_chunk_fromsyntax((yyvsp[-1].a_wsynt));
	}
#line 4741 "asn1p_y.c"
    break;

  case 149: /* ExtensionAndException: "..."  */
#line 1675 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 4754 "asn1p_y.c"
    break;

  case 150: /* ExtensionAndException: "..." '!' DefinedValue  */
#line 1683 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->value = (yyvsp[0].a_value);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 4768 "asn1p_y.c"
    break;

  case 151: /* ExtensionAndException: "..." '!' SignedNumber  */
#line 1692 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		(yyval.a_expr)->value = (yyvsp[0].a_value);
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 4782 "asn1p_y.c"
    break;

  case 153: /* TaggedType: optTagOrEncoding UntaggedType  */
#line 1706 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  {
        (yyval.a_expr) = (yyvsp[0].a_expr);
        (yyval.a_expr)->tag = (yyvsp[-1].tv_tag_enc).tag;
        if((yyvsp[-1].tv_tag_enc).enc != EC_NONE) {
            (yyval.a_expr)->encoding_control.encoding_type =
                (enum asn1p_encoding_control_type_e)(yyvsp[-1].tv_tag_enc).enc;
            if(!(yyval.a_expr)->encoding_control.encoding_reference)
                (yyval.a_expr)->encoding_control.encoding_reference =
                    strdup((yyvsp[-1].tv_tag_enc).enc == EC_JER_BASE64 ? "JER" : "XER");
        }
    }
#line 4798 "asn1p_y.c"
    break;

  case 154: /* DefinedUntaggedType: DefinedType optManyConstraints  */
#line 1720 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                       {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		/*
		 * Outer constraint for SEQUENCE OF and SET OF applies
		 * to the inner type.
		 */
		if((yyval.a_expr)->expr_type == ASN_CONSTR_SEQUENCE_OF
		|| (yyval.a_expr)->expr_type == ASN_CONSTR_SET_OF) {
			checkmem(_asn1p_merge_collection_element_constraints((yyval.a_expr), (yyvsp[0].a_constr)) == 0);
		} else {
			if((yyval.a_expr)->constraints) {
				assert(!(yyvsp[0].a_constr));
				/* Check this : optManyConstraints is not used ?! */
				asn1p_constraint_free((yyvsp[0].a_constr));
			} else {
				(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
			}
		}
	}
#line 4822 "asn1p_y.c"
    break;

  case 155: /* UntaggedType: TypeDeclaration optManyConstraints  */
#line 1742 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		/*
		 * Outer constraint for SEQUENCE OF and SET OF applies
		 * to the inner type.
		 */
		if((yyval.a_expr)->expr_type == ASN_CONSTR_SEQUENCE_OF
		|| (yyval.a_expr)->expr_type == ASN_CONSTR_SET_OF) {
			checkmem(_asn1p_merge_collection_element_constraints((yyval.a_expr), (yyvsp[0].a_constr)) == 0);
		} else {
			if((yyval.a_expr)->constraints) {
				assert(!(yyvsp[0].a_constr));
				/* Check this : optManyConstraints is not used ?! */
				asn1p_constraint_free((yyvsp[0].a_constr));
			} else {
				(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
			}
		}
	}
#line 4846 "asn1p_y.c"
    break;

  case 156: /* MaybeIndirectTaggedType: optTagOrEncoding MaybeIndirectTypeDeclaration optManyConstraints  */
#line 1764 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                     {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		(yyval.a_expr)->tag = (yyvsp[-2].tv_tag_enc).tag;
		if((yyvsp[-2].tv_tag_enc).enc != EC_NONE) {
			(yyval.a_expr)->encoding_control.encoding_type =
				(enum asn1p_encoding_control_type_e)(yyvsp[-2].tv_tag_enc).enc;
			if(!(yyval.a_expr)->encoding_control.encoding_reference)
				(yyval.a_expr)->encoding_control.encoding_reference =
					strdup((yyvsp[-2].tv_tag_enc).enc == EC_JER_BASE64 ? "JER" : "XER");
		}
		/*
		 * Outer constraint for SEQUENCE OF and SET OF applies
		 * to the inner type.
		 */
		if((yyval.a_expr)->expr_type == ASN_CONSTR_SEQUENCE_OF
		|| (yyval.a_expr)->expr_type == ASN_CONSTR_SET_OF) {
			checkmem(_asn1p_merge_collection_element_constraints((yyval.a_expr), (yyvsp[0].a_constr)) == 0);
		} else {
			if((yyval.a_expr)->constraints) {
				assert(!(yyvsp[0].a_constr));
				asn1p_constraint_free((yyvsp[0].a_constr));
			} else {
				(yyval.a_expr)->constraints = (yyvsp[0].a_constr);
			}
		}
	}
#line 4877 "asn1p_y.c"
    break;

  case 157: /* NSTD_IndirectMarker: %empty  */
#line 1793 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		(yyval.a_int) = asn1p_as_pointer ? EM_INDIRECT : 0;
		asn1p_as_pointer = 0;
	}
#line 4886 "asn1p_y.c"
    break;

  case 158: /* MaybeIndirectTypeDeclaration: NSTD_IndirectMarker TypeDeclaration  */
#line 1800 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                        {
        (yyval.a_expr) = (yyvsp[0].a_expr);
		(yyval.a_expr)->marker.flags |= (yyvsp[-1].a_int);

		if(((yyval.a_expr)->marker.flags & EM_INDIRECT)
		&& ((yyval.a_expr)->marker.flags & EM_OPTIONAL) != EM_OPTIONAL) {
			fprintf(stderr,
				"INFO: Directive <ASN1C:RepresentAsPointer> "
				"applied to %s at %s:%d\n",
				ASN_EXPR_TYPE2STR((yyval.a_expr)->expr_type)
					?  ASN_EXPR_TYPE2STR((yyval.a_expr)->expr_type)
					: "member",
				ASN_FILENAME, (yyval.a_expr)->_lineno
			);
		}
    }
#line 4907 "asn1p_y.c"
    break;

  case 162: /* ConcreteTypeDeclaration: TOK_CHOICE '{' AlternativeTypeLists '}'  */
#line 1824 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                  {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->expr_type == A1TC_INVALID);
		(yyval.a_expr)->expr_type = ASN_CONSTR_CHOICE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 4918 "asn1p_y.c"
    break;

  case 163: /* ConcreteTypeDeclaration: TOK_SEQUENCE '{' optComponentTypeLists '}'  */
#line 1830 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                     {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->expr_type == A1TC_INVALID);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SEQUENCE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 4929 "asn1p_y.c"
    break;

  case 164: /* ConcreteTypeDeclaration: TOK_SET '{' optComponentTypeLists '}'  */
#line 1836 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                {
		(yyval.a_expr) = (yyvsp[-1].a_expr);
		assert((yyval.a_expr)->expr_type == A1TC_INVALID);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SET;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 4940 "asn1p_y.c"
    break;

  case 165: /* ConcreteTypeDeclaration: TOK_SEQUENCE optSizeOrConstraint TOK_OF optIdentifier optTag MaybeIndirectTypeDeclaration  */
#line 1842 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                                                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->constraints = (yyvsp[-4].a_constr);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SEQUENCE_OF;
		(yyval.a_expr)->meta_type = AMT_TYPE;
		(yyvsp[0].a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyvsp[0].a_expr)->tag = (yyvsp[-1].a_tag);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4955 "asn1p_y.c"
    break;

  case 166: /* ConcreteTypeDeclaration: TOK_SET optSizeOrConstraint TOK_OF optIdentifier optTag MaybeIndirectTypeDeclaration  */
#line 1852 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                                               {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->constraints = (yyvsp[-4].a_constr);
		(yyval.a_expr)->expr_type = ASN_CONSTR_SET_OF;
		(yyval.a_expr)->meta_type = AMT_TYPE;
		(yyvsp[0].a_expr)->Identifier = (yyvsp[-2].tv_str);
		(yyvsp[0].a_expr)->tag = (yyvsp[-1].a_tag);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 4970 "asn1p_y.c"
    break;

  case 167: /* ConcreteTypeDeclaration: TOK_ANY  */
#line 1862 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = ASN_TYPE_ANY;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 4981 "asn1p_y.c"
    break;

  case 168: /* ConcreteTypeDeclaration: TOK_ANY TOK_DEFINED TOK_BY Identifier  */
#line 1868 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                        {
		int ret;
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = asn1p_ref_new(yylineno, currentModule);
		ret = asn1p_ref_add_component((yyval.a_expr)->reference,
			(yyvsp[0].tv_str), RLT_lowercase);
		checkmem(ret == 0);
		(yyval.a_expr)->expr_type = ASN_TYPE_ANY;
		(yyval.a_expr)->meta_type = AMT_TYPE;
		free((yyvsp[0].tv_str));
	}
#line 4998 "asn1p_y.c"
    break;

  case 169: /* ConcreteTypeDeclaration: TOK_INSTANCE TOK_OF ComplexTypeReference  */
#line 1880 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                   {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->reference = (yyvsp[0].a_ref);
		(yyval.a_expr)->expr_type = A1TC_INSTANCE;
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 5010 "asn1p_y.c"
    break;

  case 170: /* ComplexTypeReference: TOK_typereference  */
#line 1895 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		free((yyvsp[0].tv_str));
	}
#line 5023 "asn1p_y.c"
    break;

  case 171: /* ComplexTypeReference: TOK_capitalreference  */
#line 1903 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_CAPITALS);
		free((yyvsp[0].tv_str));
		checkmem(ret == 0);
	}
#line 5036 "asn1p_y.c"
    break;

  case 172: /* ComplexTypeReference: TOK_typereference '.' TypeRefName  */
#line 1911 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                            {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[-2].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 5052 "asn1p_y.c"
    break;

  case 173: /* ComplexTypeReference: TOK_capitalreference '.' TypeRefName  */
#line 1922 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                               {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[-2].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 5068 "asn1p_y.c"
    break;

  case 174: /* ComplexTypeReference: TOK_capitalreference '.' ComplexTypeReferenceAmpList  */
#line 1933 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                               {
		int ret;
		(yyval.a_ref) = (yyvsp[0].a_ref);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[-2].tv_str), RLT_CAPITALS);
		free((yyvsp[-2].tv_str));
		checkmem(ret == 0);
		/*
		 * Move the last element infront.
		 */
		{
			struct asn1p_ref_component_s tmp_comp;
			tmp_comp = (yyval.a_ref)->components[(yyval.a_ref)->comp_count-1];
			memmove(&(yyval.a_ref)->components[1],
				&(yyval.a_ref)->components[0],
				sizeof((yyval.a_ref)->components[0])
				* ((yyval.a_ref)->comp_count - 1));
			(yyval.a_ref)->components[0] = tmp_comp;
		}
	}
#line 5092 "asn1p_y.c"
    break;

  case 175: /* ComplexTypeReferenceAmpList: ComplexTypeReferenceElement  */
#line 1955 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                    {
		int ret;
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		checkmem((yyval.a_ref));
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].a_refcomp).name, (yyvsp[0].a_refcomp).lex_type);
		free((yyvsp[0].a_refcomp).name);
		checkmem(ret == 0);
	}
#line 5105 "asn1p_y.c"
    break;

  case 176: /* ComplexTypeReferenceAmpList: ComplexTypeReferenceAmpList '.' ComplexTypeReferenceElement  */
#line 1963 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                      {
		int ret;
		(yyval.a_ref) = (yyvsp[-2].a_ref);
		ret = asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].a_refcomp).name, (yyvsp[0].a_refcomp).lex_type);
		free((yyvsp[0].a_refcomp).name);
		checkmem(ret == 0);
	}
#line 5117 "asn1p_y.c"
    break;

  case 178: /* PrimitiveFieldReference: TOK_typefieldreference  */
#line 1976 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               {
		(yyval.a_refcomp).lex_type = RLT_AmpUppercase;
		(yyval.a_refcomp).name = (yyvsp[0].tv_str);
	}
#line 5126 "asn1p_y.c"
    break;

  case 179: /* PrimitiveFieldReference: TOK_valuefieldreference  */
#line 1981 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  {
		(yyval.a_refcomp).lex_type = RLT_Amplowercase;
		(yyval.a_refcomp).name = (yyvsp[0].tv_str);
	}
#line 5135 "asn1p_y.c"
    break;

  case 180: /* FieldName: TOK_typefieldreference  */
#line 1990 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               {
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_AmpUppercase);
		free((yyvsp[0].tv_str));
	}
#line 5145 "asn1p_y.c"
    break;

  case 181: /* FieldName: FieldName '.' TOK_typefieldreference  */
#line 1995 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                               {
		(yyval.a_ref) = (yyval.a_ref);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_AmpUppercase);
		free((yyvsp[0].tv_str));
	}
#line 5155 "asn1p_y.c"
    break;

  case 182: /* FieldName: FieldName '.' TOK_valuefieldreference  */
#line 2000 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                {
		(yyval.a_ref) = (yyval.a_ref);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_Amplowercase);
		free((yyvsp[0].tv_str));
	}
#line 5165 "asn1p_y.c"
    break;

  case 183: /* DefinedObjectClass: TOK_capitalreference  */
#line 2008 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             {
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_CAPITALS);
		free((yyvsp[0].tv_str));
	}
#line 5175 "asn1p_y.c"
    break;

  case 184: /* ValueAssignment: Identifier Type TOK_PPEQ Value  */
#line 2031 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                       {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		assert((yyval.a_expr)->Identifier == NULL);
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 5187 "asn1p_y.c"
    break;

  case 187: /* $@4: %empty  */
#line 2043 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
              { asn1p_lexer_hack_push_opaque_state(); }
#line 5193 "asn1p_y.c"
    break;

  case 188: /* Value: '{' $@4 Opaque  */
#line 2043 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                               {
		(yyval.a_value) = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_UNPARSED;
	}
#line 5203 "asn1p_y.c"
    break;

  case 189: /* SimpleValue: TOK_NULL  */
#line 2051 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                 {
		(yyval.a_value) = asn1p_value_fromint(0);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_NULL;
	}
#line 5213 "asn1p_y.c"
    break;

  case 190: /* SimpleValue: TOK_FALSE  */
#line 2056 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_value) = asn1p_value_fromint(0);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_FALSE;
	}
#line 5223 "asn1p_y.c"
    break;

  case 191: /* SimpleValue: TOK_TRUE  */
#line 2061 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {
		(yyval.a_value) = asn1p_value_fromint(1);
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_TRUE;
	}
#line 5233 "asn1p_y.c"
    break;

  case 197: /* DefinedValue: TypeRefName '.' Identifier  */
#line 2074 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                     {
		asn1p_ref_t *ref;
		int ret;
		ref = asn1p_ref_new(yylineno, currentModule);
		checkmem(ref);
		ret = asn1p_ref_add_component(ref, (yyvsp[-2].tv_str), RLT_UNKNOWN);
		checkmem(ret == 0);
		ret = asn1p_ref_add_component(ref, (yyvsp[0].tv_str), RLT_lowercase);
		checkmem(ret == 0);
		(yyval.a_value) = asn1p_value_fromref(ref, 0);
		checkmem((yyval.a_value));
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 5252 "asn1p_y.c"
    break;

  case 198: /* RestrictedCharacterStringValue: TOK_cstring  */
#line 2092 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_value) = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
		checkmem((yyval.a_value));
	}
#line 5261 "asn1p_y.c"
    break;

  case 199: /* RestrictedCharacterStringValue: TOK_tuple  */
#line 2096 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_TUPLE;
	}
#line 5271 "asn1p_y.c"
    break;

  case 200: /* RestrictedCharacterStringValue: TOK_quadruple  */
#line 2101 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
		(yyval.a_value)->type = ATV_QUADRUPLE;
	}
#line 5281 "asn1p_y.c"
    break;

  case 201: /* Opaque: OpaqueFirstToken  */
#line 2109 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.tv_opaque).len = (yyvsp[0].tv_opaque).len + 1;
		(yyval.tv_opaque).buf = malloc(1 + (yyval.tv_opaque).len + 1);
		checkmem((yyval.tv_opaque).buf);
		(yyval.tv_opaque).buf[0] = '{';
		memcpy((yyval.tv_opaque).buf + 1, (yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len);
		(yyval.tv_opaque).buf[(yyval.tv_opaque).len] = '\0';
		free((yyvsp[0].tv_opaque).buf);
    }
#line 5295 "asn1p_y.c"
    break;

  case 202: /* Opaque: Opaque TOK_opaque  */
#line 2118 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		int newsize = (yyvsp[-1].tv_opaque).len + (yyvsp[0].tv_opaque).len;
		char *p = malloc(newsize + 1);
		checkmem(p);
		memcpy(p         , (yyvsp[-1].tv_opaque).buf, (yyvsp[-1].tv_opaque).len);
		memcpy(p + (yyvsp[-1].tv_opaque).len, (yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len);
		p[newsize] = '\0';
		free((yyvsp[-1].tv_opaque).buf);
		free((yyvsp[0].tv_opaque).buf);
		(yyval.tv_opaque).buf = p;
		(yyval.tv_opaque).len = newsize;
	}
#line 5312 "asn1p_y.c"
    break;

  case 204: /* OpaqueFirstToken: Identifier  */
#line 2134 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                 {
        (yyval.tv_opaque).len = strlen((yyvsp[0].tv_str));
        (yyval.tv_opaque).buf = (yyvsp[0].tv_str);
    }
#line 5321 "asn1p_y.c"
    break;

  case 205: /* BasicTypeId: TOK_BOOLEAN  */
#line 2140 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    { (yyval.a_type) = ASN_BASIC_BOOLEAN; }
#line 5327 "asn1p_y.c"
    break;

  case 206: /* BasicTypeId: TOK_NULL  */
#line 2141 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   { (yyval.a_type) = ASN_BASIC_NULL; }
#line 5333 "asn1p_y.c"
    break;

  case 207: /* BasicTypeId: TOK_REAL  */
#line 2142 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   { (yyval.a_type) = ASN_BASIC_REAL; }
#line 5339 "asn1p_y.c"
    break;

  case 208: /* BasicTypeId: TOK_OCTET TOK_STRING  */
#line 2143 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               { (yyval.a_type) = ASN_BASIC_OCTET_STRING; }
#line 5345 "asn1p_y.c"
    break;

  case 209: /* BasicTypeId: TOK_OBJECT TOK_IDENTIFIER  */
#line 2144 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                    { (yyval.a_type) = ASN_BASIC_OBJECT_IDENTIFIER; }
#line 5351 "asn1p_y.c"
    break;

  case 210: /* BasicTypeId: TOK_RELATIVE_OID  */
#line 2145 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                           { (yyval.a_type) = ASN_BASIC_RELATIVE_OID; }
#line 5357 "asn1p_y.c"
    break;

  case 211: /* BasicTypeId: TOK_EMBEDDED TOK_PDV  */
#line 2147 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               { (yyval.a_type) = ASN_BASIC_EMBEDDED_PDV; }
#line 5363 "asn1p_y.c"
    break;

  case 212: /* BasicTypeId: TOK_CHARACTER TOK_STRING  */
#line 2148 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                   { (yyval.a_type) = ASN_BASIC_CHARACTER_STRING; }
#line 5369 "asn1p_y.c"
    break;

  case 213: /* BasicTypeId: TOK_UTCTime  */
#line 2149 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      { (yyval.a_type) = ASN_BASIC_UTCTime; }
#line 5375 "asn1p_y.c"
    break;

  case 214: /* BasicTypeId: TOK_GeneralizedTime  */
#line 2150 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              { (yyval.a_type) = ASN_BASIC_GeneralizedTime; }
#line 5381 "asn1p_y.c"
    break;

  case 217: /* BasicTypeId_UniverationCompatible: TOK_INTEGER  */
#line 2159 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    { (yyval.a_type) = ASN_BASIC_INTEGER; }
#line 5387 "asn1p_y.c"
    break;

  case 218: /* BasicTypeId_UniverationCompatible: TOK_ENUMERATED  */
#line 2160 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                         { (yyval.a_type) = ASN_BASIC_ENUMERATED; }
#line 5393 "asn1p_y.c"
    break;

  case 219: /* BasicTypeId_UniverationCompatible: TOK_BIT TOK_STRING  */
#line 2161 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             { (yyval.a_type) = ASN_BASIC_BIT_STRING; }
#line 5399 "asn1p_y.c"
    break;

  case 220: /* BuiltinType: BasicTypeId  */
#line 2165 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = (yyvsp[0].a_type);
		(yyval.a_expr)->meta_type = AMT_TYPE;
	}
#line 5410 "asn1p_y.c"
    break;

  case 221: /* BuiltinType: TOK_INTEGER '{' NamedNumberList '}'  */
#line 2171 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_INTEGER;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 5420 "asn1p_y.c"
    break;

  case 222: /* BuiltinType: TOK_ENUMERATED '{' Enumerations '}'  */
#line 2176 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_ENUMERATED;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 5430 "asn1p_y.c"
    break;

  case 223: /* BuiltinType: TOK_BIT TOK_STRING '{' NamedBitList '}'  */
#line 2181 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                              {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_BIT_STRING;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 5440 "asn1p_y.c"
    break;

  case 224: /* BuiltinType: TOK_ExtValue_BIT_STRING '{' IdentifierList '}'  */
#line 2186 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                     {
        (yyval.a_expr) = (yyvsp[-1].a_expr);
        (yyval.a_expr)->expr_type = ASN_BASIC_BIT_STRING;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 5450 "asn1p_y.c"
    break;

  case 225: /* BuiltinType: TOK_ExtValue_BIT_STRING '{' '}'  */
#line 2191 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
        (yyval.a_expr)->expr_type = ASN_BASIC_BIT_STRING;
        (yyval.a_expr)->meta_type = AMT_TYPE;
    }
#line 5461 "asn1p_y.c"
    break;

  case 226: /* BasicString: TOK_BMPString  */
#line 2200 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      { (yyval.a_type) = ASN_STRING_BMPString; }
#line 5467 "asn1p_y.c"
    break;

  case 227: /* BasicString: TOK_GeneralString  */
#line 2201 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		(yyval.a_type) = ASN_STRING_GeneralString;
		fprintf(stderr, "WARNING: GeneralString is not fully supported\n");
	}
#line 5476 "asn1p_y.c"
    break;

  case 228: /* BasicString: TOK_GraphicString  */
#line 2205 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		(yyval.a_type) = ASN_STRING_GraphicString;
		fprintf(stderr, "WARNING: GraphicString is not fully supported\n");
	}
#line 5485 "asn1p_y.c"
    break;

  case 229: /* BasicString: TOK_IA5String  */
#line 2209 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        { (yyval.a_type) = ASN_STRING_IA5String; }
#line 5491 "asn1p_y.c"
    break;

  case 230: /* BasicString: TOK_ISO646String  */
#line 2210 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                           { (yyval.a_type) = ASN_STRING_ISO646String; }
#line 5497 "asn1p_y.c"
    break;

  case 231: /* BasicString: TOK_NumericString  */
#line 2211 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            { (yyval.a_type) = ASN_STRING_NumericString; }
#line 5503 "asn1p_y.c"
    break;

  case 232: /* BasicString: TOK_PrintableString  */
#line 2212 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              { (yyval.a_type) = ASN_STRING_PrintableString; }
#line 5509 "asn1p_y.c"
    break;

  case 233: /* BasicString: TOK_T61String  */
#line 2213 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {
		(yyval.a_type) = ASN_STRING_T61String;
		fprintf(stderr, "WARNING: T61String is not fully supported\n");
	}
#line 5518 "asn1p_y.c"
    break;

  case 234: /* BasicString: TOK_TeletexString  */
#line 2217 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            { (yyval.a_type) = ASN_STRING_TeletexString; }
#line 5524 "asn1p_y.c"
    break;

  case 235: /* BasicString: TOK_UniversalString  */
#line 2218 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              { (yyval.a_type) = ASN_STRING_UniversalString; }
#line 5530 "asn1p_y.c"
    break;

  case 236: /* BasicString: TOK_UTF8String  */
#line 2219 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                         { (yyval.a_type) = ASN_STRING_UTF8String; }
#line 5536 "asn1p_y.c"
    break;

  case 237: /* BasicString: TOK_VideotexString  */
#line 2220 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             {
		(yyval.a_type) = ASN_STRING_VideotexString;
		fprintf(stderr, "WARNING: VideotexString is not fully supported\n");
	}
#line 5545 "asn1p_y.c"
    break;

  case 238: /* BasicString: TOK_VisibleString  */
#line 2224 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            { (yyval.a_type) = ASN_STRING_VisibleString; }
#line 5551 "asn1p_y.c"
    break;

  case 239: /* BasicString: TOK_ObjectDescriptor  */
#line 2225 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               { (yyval.a_type) = ASN_STRING_ObjectDescriptor; }
#line 5557 "asn1p_y.c"
    break;

  case 244: /* optConstraint: %empty  */
#line 2237 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_constr) = 0; }
#line 5563 "asn1p_y.c"
    break;

  case 246: /* optManyConstraints: %empty  */
#line 2242 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_constr) = 0; }
#line 5569 "asn1p_y.c"
    break;

  case 248: /* optSizeOrConstraint: %empty  */
#line 2247 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_constr) = 0; }
#line 5575 "asn1p_y.c"
    break;

  case 250: /* optSizeOrConstraint: SizeConstraint  */
#line 2249 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                         {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_SET, (yyvsp[0].a_constr), 0);
    }
#line 5583 "asn1p_y.c"
    break;

  case 251: /* Constraint: '(' ConstraintSpec ')'  */
#line 2255 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                           {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_SET, (yyvsp[-1].a_constr), 0);
    }
#line 5591 "asn1p_y.c"
    break;

  case 253: /* ManyConstraints: ManyConstraints Constraint  */
#line 2262 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                     {
        if((yyvsp[0].a_constr)->type == ACT_CA_SET && (yyvsp[0].a_constr)->el_count == 1) {
            CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_SET, (yyvsp[-1].a_constr), (yyvsp[0].a_constr)->elements[0]);
        } else {
            CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_SET, (yyvsp[-1].a_constr), (yyvsp[0].a_constr));
        }
	}
#line 5603 "asn1p_y.c"
    break;

  case 257: /* ElementSetSpecs: "..."  */
#line 2276 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		(yyval.a_constr)->type = ACT_EL_EXT;
	}
#line 5612 "asn1p_y.c"
    break;

  case 259: /* ElementSetSpecs: ElementSetSpec ',' "..."  */
#line 2281 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
       asn1p_constraint_t *ct;
       ct = asn1p_constraint_new(yylineno, currentModule);
       ct->type = ACT_EL_EXT;
       CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, (yyvsp[-2].a_constr), ct);
   }
#line 5623 "asn1p_y.c"
    break;

  case 260: /* ElementSetSpecs: ElementSetSpec ',' "..." ',' ElementSetSpec  */
#line 2287 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                         {
       asn1p_constraint_t *ct;
       ct = asn1p_constraint_new(yylineno, currentModule);
       ct->type = ACT_EL_EXT;
       CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, (yyvsp[-4].a_constr), ct);
       ct = (yyval.a_constr);
       CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, ct, (yyvsp[0].a_constr));
   }
#line 5636 "asn1p_y.c"
    break;

  case 262: /* ElementSetSpec: TOK_ALL TOK_EXCEPT Elements  */
#line 2299 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_AEX, (yyvsp[0].a_constr), 0);
	}
#line 5644 "asn1p_y.c"
    break;

  case 264: /* Unions: Unions UnionMark Intersections  */
#line 2306 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_UNI, (yyvsp[-2].a_constr), (yyvsp[0].a_constr));
	}
#line 5652 "asn1p_y.c"
    break;

  case 266: /* Intersections: Intersections IntersectionMark IntersectionElements  */
#line 2313 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                               {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_INT, (yyvsp[-2].a_constr), (yyvsp[0].a_constr));
	}
#line 5660 "asn1p_y.c"
    break;

  case 268: /* IntersectionElements: Elements TOK_EXCEPT Elements  */
#line 2321 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                       {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_EXC, (yyvsp[-2].a_constr), (yyvsp[0].a_constr));
	}
#line 5668 "asn1p_y.c"
    break;

  case 270: /* Elements: '(' ElementSetSpec ')'  */
#line 2328 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             {
        int ret;
        (yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
        checkmem((yyval.a_constr));
        (yyval.a_constr)->type = ACT_CA_SET;
        ret = asn1p_constraint_insert((yyval.a_constr), (yyvsp[-1].a_constr));
        checkmem(ret == 0);
    }
#line 5681 "asn1p_y.c"
    break;

  case 271: /* SubtypeElements: SingleValue  */
#line 2339 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_EL_VALUE;
		(yyval.a_constr)->value = (yyvsp[0].a_value);
	}
#line 5692 "asn1p_y.c"
    break;

  case 272: /* SubtypeElements: ContainedSubtype  */
#line 2345 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                           {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_EL_TYPE;
		(yyval.a_constr)->containedSubtype = (yyvsp[0].a_value);
	}
#line 5703 "asn1p_y.c"
    break;

  case 278: /* PermittedAlphabet: TOK_FROM Constraint  */
#line 2361 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CT_FROM, (yyvsp[0].a_constr), 0);
	}
#line 5711 "asn1p_y.c"
    break;

  case 279: /* SizeConstraint: TOK_SIZE Constraint  */
#line 2366 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CT_SIZE, (yyvsp[0].a_constr), 0);
	}
#line 5719 "asn1p_y.c"
    break;

  case 280: /* PatternConstraint: TOK_PATTERN TOK_cstring  */
#line 2371 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		(yyval.a_constr)->type = ACT_CT_PATTERN;
		(yyval.a_constr)->value = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
	}
#line 5729 "asn1p_y.c"
    break;

  case 281: /* PatternConstraint: TOK_PATTERN Identifier  */
#line 2376 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                 {
		asn1p_ref_t *ref;
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		(yyval.a_constr)->type = ACT_CT_PATTERN;
		ref = asn1p_ref_new(yylineno, currentModule);
		asn1p_ref_add_component(ref, (yyvsp[0].tv_str), RLT_lowercase);
		(yyval.a_constr)->value = asn1p_value_fromref(ref, 0);
		free((yyvsp[0].tv_str));
	}
#line 5743 "asn1p_y.c"
    break;

  case 282: /* ValueRange: LowerEndValue ConstraintRangeSpec UpperEndValue  */
#line 2388 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                    {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = (yyvsp[-1].a_ctype);
		(yyval.a_constr)->range_start = (yyvsp[-2].a_value);
		(yyval.a_constr)->range_stop = (yyvsp[0].a_value);
    }
#line 5755 "asn1p_y.c"
    break;

  case 284: /* LowerEndValue: TOK_MIN  */
#line 2398 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
              {
		(yyval.a_value) = asn1p_value_fromint(-123);
		(yyval.a_value)->type = ATV_MIN;
    }
#line 5764 "asn1p_y.c"
    break;

  case 286: /* UpperEndValue: TOK_MAX  */
#line 2405 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
              {
		(yyval.a_value) = asn1p_value_fromint(321);
		(yyval.a_value)->type = ATV_MAX;
    }
#line 5773 "asn1p_y.c"
    break;

  case 288: /* BitStringValue: TOK_bstring  */
#line 2413 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_value) = _convert_bitstring2binary((yyvsp[0].tv_str), 'B');
		checkmem((yyval.a_value));
		free((yyvsp[0].tv_str));
	}
#line 5783 "asn1p_y.c"
    break;

  case 289: /* BitStringValue: TOK_hstring  */
#line 2418 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_value) = _convert_bitstring2binary((yyvsp[0].tv_str), 'H');
		checkmem((yyval.a_value));
		free((yyvsp[0].tv_str));
	}
#line 5793 "asn1p_y.c"
    break;

  case 290: /* ContainedSubtype: TOK_INCLUDES Type  */
#line 2426 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_value) = asn1p_value_fromtype((yyvsp[0].a_expr));
		checkmem((yyval.a_value));
		asn1p_expr_free((yyvsp[0].a_expr));
    }
#line 5803 "asn1p_y.c"
    break;

  case 291: /* ContainedSubtype: DefinedUntaggedType  */
#line 2432 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		(yyval.a_value) = asn1p_value_fromtype((yyvsp[0].a_expr));
		checkmem((yyval.a_value));
		asn1p_expr_free((yyvsp[0].a_expr));
    }
#line 5813 "asn1p_y.c"
    break;

  case 292: /* InnerTypeConstraints: TOK_WITH TOK_COMPONENT SingleTypeConstraint  */
#line 2444 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                    {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CT_WCOMP, (yyvsp[0].a_constr), 0);
	}
#line 5821 "asn1p_y.c"
    break;

  case 293: /* InnerTypeConstraints: TOK_WITH TOK_COMPONENTS MultipleTypeConstraints  */
#line 2447 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                          {
        assert((yyvsp[0].a_constr)->type == ACT_CA_CSV);
        (yyvsp[0].a_constr)->type = ACT_CT_WCOMPS;
        (yyval.a_constr) = (yyvsp[0].a_constr);
	}
#line 5831 "asn1p_y.c"
    break;

  case 297: /* FullSpecification: '{' TypeConstraints '}'  */
#line 2455 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                           { (yyval.a_constr) = (yyvsp[-1].a_constr); }
#line 5837 "asn1p_y.c"
    break;

  case 298: /* PartialSpecification: '{' "..." ',' TypeConstraints '}'  */
#line 2457 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                              {
        assert((yyvsp[-1].a_constr)->type == ACT_CA_CSV);
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
        (yyval.a_constr)->type = ACT_CA_CSV;
		asn1p_constraint_t *ct = asn1p_constraint_new(yylineno, currentModule);
		checkmem((yyval.a_constr));
		ct->type = ACT_EL_EXT;
        asn1p_constraint_insert((yyval.a_constr), ct);
        for(unsigned i = 0; i < (yyvsp[-1].a_constr)->el_count; i++) {
            asn1p_constraint_insert((yyval.a_constr), (yyvsp[-1].a_constr)->elements[i]);
            (yyvsp[-1].a_constr)->elements[i] = NULL;
        }
        asn1p_constraint_free((yyvsp[-1].a_constr));
    }
#line 5856 "asn1p_y.c"
    break;

  case 299: /* TypeConstraints: NamedConstraint  */
#line 2472 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
        (yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
        (yyval.a_constr)->type = ACT_CA_CSV;
        asn1p_constraint_insert((yyval.a_constr), (yyvsp[0].a_constr));
    }
#line 5866 "asn1p_y.c"
    break;

  case 300: /* TypeConstraints: TypeConstraints ',' NamedConstraint  */
#line 2477 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
        (yyval.a_constr) = (yyvsp[-2].a_constr);
        asn1p_constraint_insert((yyval.a_constr), (yyvsp[0].a_constr));
	}
#line 5875 "asn1p_y.c"
    break;

  case 301: /* NamedConstraint: IdentifierAsValue optConstraint optPresenceConstraint  */
#line 2483 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                              {
        (yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
        checkmem((yyval.a_constr));
        (yyval.a_constr)->type = ACT_EL_VALUE;
        (yyval.a_constr)->value = (yyvsp[-2].a_value);
        if((yyvsp[-1].a_constr)) asn1p_constraint_insert((yyval.a_constr), (yyvsp[-1].a_constr));
        (yyval.a_constr)->presence = (yyvsp[0].a_pres);
    }
#line 5888 "asn1p_y.c"
    break;

  case 302: /* optPresenceConstraint: %empty  */
#line 2497 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_pres) = ACPRES_DEFAULT; }
#line 5894 "asn1p_y.c"
    break;

  case 303: /* optPresenceConstraint: PresenceConstraint  */
#line 2498 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                             { (yyval.a_pres) = (yyvsp[0].a_pres); }
#line 5900 "asn1p_y.c"
    break;

  case 304: /* PresenceConstraint: TOK_PRESENT  */
#line 2502 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_pres) = ACPRES_PRESENT;
	}
#line 5908 "asn1p_y.c"
    break;

  case 305: /* PresenceConstraint: TOK_ABSENT  */
#line 2505 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.a_pres) = ACPRES_ABSENT;
	}
#line 5916 "asn1p_y.c"
    break;

  case 306: /* PresenceConstraint: TOK_OPTIONAL  */
#line 2508 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_pres) = ACPRES_OPTIONAL;
	}
#line 5924 "asn1p_y.c"
    break;

  case 310: /* $@5: %empty  */
#line 2523 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                { asn1p_lexer_hack_push_opaque_state(); }
#line 5930 "asn1p_y.c"
    break;

  case 311: /* UserDefinedConstraint: TOK_CONSTRAINED TOK_BY '{' $@5 Opaque  */
#line 2523 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                           {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_CT_CTDBY;
		(yyval.a_constr)->value = asn1p_value_frombuf((yyvsp[0].tv_opaque).buf, (yyvsp[0].tv_opaque).len, 0);
		checkmem((yyval.a_constr)->value);
		(yyval.a_constr)->value->type = ATV_UNPARSED;
	}
#line 5943 "asn1p_y.c"
    break;

  case 312: /* ContentsConstraint: TOK_CONTAINING Type  */
#line 2534 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		(yyval.a_constr)->type = ACT_CT_CTNG;
		(yyval.a_constr)->value = asn1p_value_fromtype((yyvsp[0].a_expr));
		asn1p_expr_free((yyvsp[0].a_expr));
	}
#line 5954 "asn1p_y.c"
    break;

  case 313: /* ConstraintRangeSpec: ".."  */
#line 2543 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_RANGE; }
#line 5960 "asn1p_y.c"
    break;

  case 314: /* ConstraintRangeSpec: ".." '<'  */
#line 2544 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_RLRANGE; }
#line 5966 "asn1p_y.c"
    break;

  case 315: /* ConstraintRangeSpec: '<' ".."  */
#line 2545 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_LLRANGE; }
#line 5972 "asn1p_y.c"
    break;

  case 316: /* ConstraintRangeSpec: '<' ".." '<'  */
#line 2546 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                { (yyval.a_ctype) = ACT_EL_ULRANGE; }
#line 5978 "asn1p_y.c"
    break;

  case 317: /* TableConstraint: SimpleTableConstraint  */
#line 2549 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              {
		(yyval.a_constr) = (yyvsp[0].a_constr);
	}
#line 5986 "asn1p_y.c"
    break;

  case 318: /* TableConstraint: ComponentRelationConstraint  */
#line 2552 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
		(yyval.a_constr) = (yyvsp[0].a_constr);
	}
#line 5994 "asn1p_y.c"
    break;

  case 319: /* SimpleTableConstraint: '{' TypeRefName '}'  */
#line 2561 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		asn1p_ref_t *ref = asn1p_ref_new(yylineno, currentModule);
		asn1p_constraint_t *ct;
		int ret;
		ret = asn1p_ref_add_component(ref, (yyvsp[-1].tv_str), 0);
		checkmem(ret == 0);
		ct = asn1p_constraint_new(yylineno, currentModule);
		checkmem((yyval.a_constr));
		ct->type = ACT_EL_VALUE;
		ct->value = asn1p_value_fromref(ref, 0);
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CRC, ct, 0);
		free((yyvsp[-1].tv_str));
	}
#line 6012 "asn1p_y.c"
    break;

  case 320: /* ComponentRelationConstraint: SimpleTableConstraint '{' AtNotationList '}'  */
#line 2577 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                     {
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CRC, (yyvsp[-3].a_constr), (yyvsp[-1].a_constr));
	}
#line 6020 "asn1p_y.c"
    break;

  case 321: /* AtNotationList: AtNotationElement  */
#line 2583 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		(yyval.a_constr) = asn1p_constraint_new(yylineno, currentModule);
		checkmem((yyval.a_constr));
		(yyval.a_constr)->type = ACT_EL_VALUE;
		(yyval.a_constr)->value = asn1p_value_fromref((yyvsp[0].a_ref), 0);
	}
#line 6031 "asn1p_y.c"
    break;

  case 322: /* AtNotationList: AtNotationList ',' AtNotationElement  */
#line 2589 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                               {
		asn1p_constraint_t *ct;
		ct = asn1p_constraint_new(yylineno, currentModule);
		checkmem(ct);
		ct->type = ACT_EL_VALUE;
		ct->value = asn1p_value_fromref((yyvsp[0].a_ref), 0);
		CONSTRAINT_INSERT((yyval.a_constr), ACT_CA_CSV, (yyvsp[-2].a_constr), ct);
	}
#line 6044 "asn1p_y.c"
    break;

  case 323: /* AtNotationElement: '@' ComponentIdList  */
#line 2603 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		char *p = malloc(strlen((yyvsp[0].tv_str)) + 2);
		int ret;
		*p = '@';
		strcpy(p + 1, (yyvsp[0].tv_str));
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		ret = asn1p_ref_add_component((yyval.a_ref), p, 0);
		checkmem(ret == 0);
		free(p);
		free((yyvsp[0].tv_str));
	}
#line 6060 "asn1p_y.c"
    break;

  case 324: /* AtNotationElement: '@' '.' ComponentIdList  */
#line 2614 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                  {
		char *p = malloc(strlen((yyvsp[0].tv_str)) + 3);
		int ret;
		p[0] = '@';
		p[1] = '.';
		strcpy(p + 2, (yyvsp[0].tv_str));
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		ret = asn1p_ref_add_component((yyval.a_ref), p, 0);
		checkmem(ret == 0);
		free(p);
		free((yyvsp[0].tv_str));
	}
#line 6077 "asn1p_y.c"
    break;

  case 325: /* ComponentIdList: Identifier  */
#line 2630 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 6085 "asn1p_y.c"
    break;

  case 326: /* ComponentIdList: ComponentIdList '.' Identifier  */
#line 2633 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                         {
		int l1 = strlen((yyvsp[-2].tv_str));
		int l3 = strlen((yyvsp[0].tv_str));
		(yyval.tv_str) = malloc(l1 + 1 + l3 + 1);
		memcpy((yyval.tv_str), (yyvsp[-2].tv_str), l1);
		(yyval.tv_str)[l1] = '.';
		memcpy((yyval.tv_str) + l1 + 1, (yyvsp[0].tv_str), l3);
		(yyval.tv_str)[l1 + 1 + l3] = '\0';
		free((yyvsp[-2].tv_str));
		free((yyvsp[0].tv_str));
	}
#line 6101 "asn1p_y.c"
    break;

  case 327: /* optMarker: %empty  */
#line 2653 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		(yyval.a_marker).flags = EM_NOMARK;
		(yyval.a_marker).default_value = 0;
	}
#line 6110 "asn1p_y.c"
    break;

  case 328: /* optMarker: Marker  */
#line 2657 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                 { (yyval.a_marker) = (yyvsp[0].a_marker); }
#line 6116 "asn1p_y.c"
    break;

  case 329: /* Marker: TOK_OPTIONAL  */
#line 2661 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.a_marker).flags = EM_OPTIONAL | EM_INDIRECT;
		(yyval.a_marker).default_value = 0;
	}
#line 6125 "asn1p_y.c"
    break;

  case 330: /* Marker: TOK_DEFAULT Value  */
#line 2665 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                            {
		(yyval.a_marker).flags = EM_DEFAULT;
		(yyval.a_marker).default_value = (yyvsp[0].a_value);
	}
#line 6134 "asn1p_y.c"
    break;

  case 331: /* IdentifierList: IdentifierElement  */
#line 2672 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
    }
#line 6144 "asn1p_y.c"
    break;

  case 332: /* IdentifierList: IdentifierList ',' IdentifierElement  */
#line 2677 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                           {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
    }
#line 6153 "asn1p_y.c"
    break;

  case 333: /* IdentifierElement: Identifier  */
#line 2683 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
               {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
    }
#line 6165 "asn1p_y.c"
    break;

  case 334: /* NamedNumberList: NamedNumber  */
#line 2692 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 6175 "asn1p_y.c"
    break;

  case 335: /* NamedNumberList: NamedNumberList ',' NamedNumber  */
#line 2697 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 6184 "asn1p_y.c"
    break;

  case 336: /* NamedNumber: Identifier '(' SignedNumber ')'  */
#line 2704 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 6197 "asn1p_y.c"
    break;

  case 337: /* NamedNumber: Identifier '(' DefinedValue ')'  */
#line 2712 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 6210 "asn1p_y.c"
    break;

  case 338: /* NamedBitList: NamedBit  */
#line 2722 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                 {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 6220 "asn1p_y.c"
    break;

  case 339: /* NamedBitList: NamedBitList ',' NamedBit  */
#line 2727 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                    {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 6229 "asn1p_y.c"
    break;

  case 340: /* NamedBit: Identifier '(' "number" ')'  */
#line 2734 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                      {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = asn1p_value_fromint((yyvsp[-1].a_int));
	}
#line 6242 "asn1p_y.c"
    break;

  case 341: /* NamedBit: Identifier '(' DefinedValue ')'  */
#line 2742 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 6255 "asn1p_y.c"
    break;

  case 342: /* Enumerations: UniverationList  */
#line 2752 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                    {
		(yyval.a_expr) = (yyvsp[0].a_expr);
        asn1p_expr_t *first_memb = TQ_FIRST(&((yyval.a_expr)->members));
        if(first_memb) {
            if(first_memb->expr_type == A1TC_EXTENSIBLE) {
                return yyerror(param,
                    "The ENUMERATION cannot start with extension (...).");
            }
        } else {
            return yyerror(param,
                "The ENUMERATION list cannot be empty.");
        }
    }
#line 6273 "asn1p_y.c"
    break;

  case 343: /* UniverationList: UniverationElement  */
#line 2767 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                           {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 6283 "asn1p_y.c"
    break;

  case 344: /* UniverationList: UniverationList ',' UniverationElement  */
#line 2772 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                 {
		(yyval.a_expr) = (yyvsp[-2].a_expr);
		asn1p_expr_add((yyval.a_expr), (yyvsp[0].a_expr));
	}
#line 6292 "asn1p_y.c"
    break;

  case 345: /* UniverationElement: Identifier  */
#line 2779 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[0].tv_str);
	}
#line 6304 "asn1p_y.c"
    break;

  case 346: /* UniverationElement: Identifier '(' SignedNumber ')'  */
#line 2786 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 6317 "asn1p_y.c"
    break;

  case 347: /* UniverationElement: Identifier '(' DefinedValue ')'  */
#line 2794 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                          {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->Identifier = (yyvsp[-3].tv_str);
		(yyval.a_expr)->value = (yyvsp[-1].a_value);
	}
#line 6330 "asn1p_y.c"
    break;

  case 348: /* UniverationElement: SignedNumber  */
#line 2802 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->expr_type = A1TC_UNIVERVAL;
		(yyval.a_expr)->meta_type = AMT_VALUE;
		(yyval.a_expr)->value = (yyvsp[0].a_value);
	}
#line 6342 "asn1p_y.c"
    break;

  case 349: /* UniverationElement: "..."  */
#line 2809 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        {
		(yyval.a_expr) = NEW_EXPR();
		checkmem((yyval.a_expr));
		(yyval.a_expr)->Identifier = strdup("...");
		checkmem((yyval.a_expr)->Identifier);
		(yyval.a_expr)->expr_type = A1TC_EXTENSIBLE;
		(yyval.a_expr)->meta_type = AMT_VALUE;
	}
#line 6355 "asn1p_y.c"
    break;

  case 350: /* SignedNumber: "number"  */
#line 2820 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                   {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
	}
#line 6364 "asn1p_y.c"
    break;

  case 351: /* SignedNumber: "negative number"  */
#line 2824 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                              {
		(yyval.a_value) = asn1p_value_fromint((yyvsp[0].a_int));
		checkmem((yyval.a_value));
	}
#line 6373 "asn1p_y.c"
    break;

  case 352: /* RealValue: TOK_realnumber  */
#line 2831 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		(yyval.a_value) = asn1p_value_fromdouble((yyvsp[0].a_dbl));
		checkmem((yyval.a_value));
	}
#line 6382 "asn1p_y.c"
    break;

  case 353: /* optTag: %empty  */
#line 2862 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { memset(&(yyval.a_tag), 0, sizeof((yyval.a_tag))); }
#line 6388 "asn1p_y.c"
    break;

  case 354: /* optTag: Tag  */
#line 2863 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
              { (yyval.a_tag) = (yyvsp[0].a_tag); }
#line 6394 "asn1p_y.c"
    break;

  case 355: /* optTagOrEncoding: %empty  */
#line 2880 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        {
		memset(&(yyval.tv_tag_enc), 0, sizeof((yyval.tv_tag_enc)));
		(yyval.tv_tag_enc).enc = EC_NONE;
	}
#line 6403 "asn1p_y.c"
    break;

  case 356: /* optTagOrEncoding: Tag  */
#line 2884 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
              {
		(yyval.tv_tag_enc).tag = (yyvsp[0].a_tag);
		(yyval.tv_tag_enc).enc = EC_NONE;
	}
#line 6412 "asn1p_y.c"
    break;

  case 357: /* optTagOrEncoding: '[' TOK_capitalreference ']'  */
#line 2888 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                       {
		/* Bare instruction prefixes use the XER encoding reference. */
		memset(&(yyval.tv_tag_enc), 0, sizeof((yyval.tv_tag_enc)));
		if(strcmp((yyvsp[-1].tv_str), "BASE64") == 0) {
			(yyval.tv_tag_enc).enc = EC_XER_BASE64;
		} else if(strcmp((yyvsp[-1].tv_str), "TEXT") == 0) {
			(yyval.tv_tag_enc).enc = EC_XER_TEXT;
		} else if(strcmp((yyvsp[-1].tv_str), "DECIMAL") == 0) {
			(yyval.tv_tag_enc).enc = EC_XER_DECIMAL;
		} else {
			fprintf(stderr,
				"WARNING: Unknown XER encoding instruction [%s]"
				" at %s:%d, ignored\n",
				(yyvsp[-1].tv_str), ASN_FILENAME, yylineno);
			(yyval.tv_tag_enc).enc = EC_NONE;
		}
		free((yyvsp[-1].tv_str));
	}
#line 6435 "asn1p_y.c"
    break;

  case 358: /* optTagOrEncoding: '[' TOK_capitalreference ':' TOK_capitalreference ']'  */
#line 2906 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                                                {
		/* Qualified encoding instruction prefix. */
		memset(&(yyval.tv_tag_enc), 0, sizeof((yyval.tv_tag_enc)));
		if(strcmp((yyvsp[-3].tv_str), "XER") == 0 && strcmp((yyvsp[-1].tv_str), "BASE64") == 0) {
			(yyval.tv_tag_enc).enc = EC_XER_BASE64;
		} else if(strcmp((yyvsp[-3].tv_str), "XER") == 0 && strcmp((yyvsp[-1].tv_str), "TEXT") == 0) {
			(yyval.tv_tag_enc).enc = EC_XER_TEXT;
		} else if(strcmp((yyvsp[-3].tv_str), "XER") == 0 && strcmp((yyvsp[-1].tv_str), "DECIMAL") == 0) {
			(yyval.tv_tag_enc).enc = EC_XER_DECIMAL;
		} else if(strcmp((yyvsp[-3].tv_str), "JER") == 0 && strcmp((yyvsp[-1].tv_str), "BASE64") == 0) {
			(yyval.tv_tag_enc).enc = EC_JER_BASE64;
		} else {
			fprintf(stderr,
				"WARNING: Unknown XER encoding instruction"
				" [%s:%s] at %s:%d, ignored\n",
				(yyvsp[-3].tv_str), (yyvsp[-1].tv_str), ASN_FILENAME, yylineno);
			(yyval.tv_tag_enc).enc = EC_NONE;
		}
		free((yyvsp[-3].tv_str));
		free((yyvsp[-1].tv_str));
	}
#line 6461 "asn1p_y.c"
    break;

  case 359: /* Tag: TagTypeValue TagPlicit  */
#line 2930 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               {
		(yyval.a_tag) = (yyvsp[-1].a_tag);
		(yyval.a_tag).tag_mode = (yyvsp[0].a_tag).tag_mode;
	}
#line 6470 "asn1p_y.c"
    break;

  case 360: /* TagTypeValue: '[' TagClass "number" ']'  */
#line 2937 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                                    {
		(yyval.a_tag) = (yyvsp[-2].a_tag);
		(yyval.a_tag).tag_value = (yyvsp[-1].a_int);
	}
#line 6479 "asn1p_y.c"
    break;

  case 361: /* TagClass: %empty  */
#line 2943 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_tag).tag_class = TC_CONTEXT_SPECIFIC; }
#line 6485 "asn1p_y.c"
    break;

  case 362: /* TagClass: TOK_UNIVERSAL  */
#line 2944 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                        { (yyval.a_tag).tag_class = TC_UNIVERSAL; }
#line 6491 "asn1p_y.c"
    break;

  case 363: /* TagClass: TOK_APPLICATION  */
#line 2945 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          { (yyval.a_tag).tag_class = TC_APPLICATION; }
#line 6497 "asn1p_y.c"
    break;

  case 364: /* TagClass: TOK_PRIVATE  */
#line 2946 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                      { (yyval.a_tag).tag_class = TC_PRIVATE; }
#line 6503 "asn1p_y.c"
    break;

  case 365: /* TagPlicit: %empty  */
#line 2950 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.a_tag).tag_mode = TM_DEFAULT; }
#line 6509 "asn1p_y.c"
    break;

  case 366: /* TagPlicit: TOK_IMPLICIT  */
#line 2951 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       { (yyval.a_tag).tag_mode = TM_IMPLICIT; }
#line 6515 "asn1p_y.c"
    break;

  case 367: /* TagPlicit: TOK_EXPLICIT  */
#line 2952 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       { (yyval.a_tag).tag_mode = TM_EXPLICIT; }
#line 6521 "asn1p_y.c"
    break;

  case 368: /* TypeRefName: TOK_typereference  */
#line 2956 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		checkmem((yyvsp[0].tv_str));
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 6530 "asn1p_y.c"
    break;

  case 369: /* TypeRefName: TOK_capitalreference  */
#line 2960 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                               {
		checkmem((yyvsp[0].tv_str));
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 6539 "asn1p_y.c"
    break;

  case 370: /* optIdentifier: %empty  */
#line 2968 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
        { (yyval.tv_str) = 0; }
#line 6545 "asn1p_y.c"
    break;

  case 371: /* optIdentifier: Identifier  */
#line 2969 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                     {
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 6553 "asn1p_y.c"
    break;

  case 372: /* Identifier: "identifier"  */
#line 2975 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                       {
		checkmem((yyvsp[0].tv_str));
		(yyval.tv_str) = (yyvsp[0].tv_str);
	}
#line 6562 "asn1p_y.c"
    break;

  case 373: /* IdentifierAsReference: Identifier  */
#line 2982 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
               {
		(yyval.a_ref) = asn1p_ref_new(yylineno, currentModule);
		asn1p_ref_add_component((yyval.a_ref), (yyvsp[0].tv_str), RLT_lowercase);
		free((yyvsp[0].tv_str));
    }
#line 6572 "asn1p_y.c"
    break;

  case 374: /* IdentifierAsValue: IdentifierAsReference  */
#line 2989 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"
                          {
		(yyval.a_value) = asn1p_value_fromref((yyvsp[0].a_ref), 0);
    }
#line 6580 "asn1p_y.c"
    break;


#line 6584 "asn1p_y.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (param, YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, param);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, param);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (param, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, param);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, param);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 2993 "/home/ubuntu/openairinterface5g/ext/asn1c/source/libasn1parser/asn1p_y.y"



/*
 * Convert Xstring ('0101'B or '5'H) to the binary vector.
 */
static asn1p_value_t *
_convert_bitstring2binary(char *str, int base) {
	asn1p_value_t *val;
	int slen;
	int memlen;
	int baselen;
	int bits;
	uint8_t *binary_vector;
	uint8_t *bv_ptr;
	uint8_t cur_val;

	assert(str);
	assert(str[0] == '\'');

	switch(base) {
	case 'B':
		baselen = 1;
		break;
	case 'H':
		baselen = 4;
		break;
	default:
		assert(base == 'B' || base == 'H');
		errno = EINVAL;
		return NULL;
	}

	slen = strlen(str);
	assert(str[slen - 1] == base);
	assert(str[slen - 2] == '\'');

	memlen = slen / (8 / baselen);	/* Conservative estimate */

	bv_ptr = binary_vector = malloc(memlen + 1);
	if(bv_ptr == NULL) {
		/* ENOMEM */
		return NULL;
	}

	cur_val = 0;
	bits = 0;
	while(*(++str) != '\'') {
		switch(baselen) {
		case 1:
			switch(*str) {
			case '1':
				cur_val |= 1 << (7 - (bits % 8));
			case '0':
				break;
			default:
				assert(!"_y UNREACH1");
			case ' ': case '\r': case '\n':
				continue;
			}
			break;
		case 4:
			switch(*str) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				cur_val |= (*str - '0') << (4 - (bits % 8));
				break;
			case 'A': case 'B': case 'C':
			case 'D': case 'E': case 'F':
				cur_val |= ((*str - 'A') + 10)
					<< (4 - (bits % 8));
				break;
			default:
				assert(!"_y UNREACH2");
			case ' ': case '\r': case '\n':
				continue;
			}
			break;
		}

		bits += baselen;
		if((bits % 8) == 0) {
			*bv_ptr++ = cur_val;
			cur_val = 0;
		}
	}

	*bv_ptr = cur_val;
	assert((bv_ptr - binary_vector) <= memlen);

	val = asn1p_value_frombits(binary_vector, bits, 0);
	if(val == NULL) {
		free(binary_vector);
	}

	return val;
}

/*
 * For unnamed types (used in old X.208 compliant modules)
 * generate some sort of interim names, to not to force human being to fix
 * the specification's compliance to modern ASN.1 standards.
 */
static void
_fixup_anonymous_identifier(asn1p_expr_t *expr) {
	char *p;
	assert(expr->Identifier == 0);

	/*
	 * Try to figure out the type name
	 * without going too much into details
	 */
	expr->Identifier = ASN_EXPR_TYPE2STR(expr->expr_type);
	if(expr->reference && expr->reference->comp_count > 0)
		expr->Identifier = expr->reference->components[0].name;

	fprintf(stderr,
		"WARNING: Line %d: expected lower-case member identifier, "
		"found an unnamed %s.\n"
		"WARNING: Obsolete X.208 syntax detected, "
		"please give the member a name.\n",
		yylineno, expr->Identifier ? expr->Identifier : "type");

	if(!expr->Identifier)
		expr->Identifier = "unnamed";
	expr->Identifier = strdup(expr->Identifier);
	assert(expr->Identifier);
	/* Make a lowercase identifier from the type name */
	for(p = expr->Identifier; *p; p++) {
		switch(*p) {
		case 'A' ... 'Z': *p += 32; break;
		case ' ': *p = '_'; break;
		case '-': *p = '_'; break;
		}
	}
	fprintf(stderr, "NOTE: Assigning temporary identifier \"%s\". "
			"Name clash may occur later.\n",
		expr->Identifier);
}

static char *
_encoding_control_join(char *a, char *b) {
	size_t alen = strlen(a);
	size_t blen = strlen(b);
	char *r = malloc(alen + blen + 2);
	if(r) {
		memcpy(r, a, alen);
		r[alen] = '.';
		memcpy(r + alen + 1, b, blen + 1);
	}
	free(a);
	free(b);
	return r;
}

static char *
_encoding_control_cstring(char *buf, int len) {
	char *r = malloc((size_t)len + 1);
	if(r) {
		memcpy(r, buf, (size_t)len);
		r[len] = '\0';
	}
	free(buf);
	return r;
}

int
yyerror(void **param, const char *msg) {
	(void)param;
	extern char *asn1p_text;
	fprintf(stderr,
		"ASN.1 grammar parse error "
		"near %s:%d (token \"%s\"): %s\n",
		ASN_FILENAME, yylineno, asn1p_text, msg);
	return -1;
}
