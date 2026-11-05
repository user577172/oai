/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

#line 334 "asn1p_y.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE asn1p_lval;


int asn1p_parse (void **param);


#endif /* !YY_ASN1P_ASN_P_Y_H_INCLUDED  */
