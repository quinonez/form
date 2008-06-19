/** @file structs.h
 *
 *  Contains definitions for global structs.
 *
 *  !!!CAUTION!!!
 *  Changes in this file will most likely have consequences for the recovery
 *  mechanism (see checkpoint.c). You need to care for the code in checkpoint.c
 *  as well and modify the code there accordingly!
 *
 *  The marker [D] is used in comments in this file to mark pointers to which
 *  dynamically allocated memory is assigned by a call to malloc() during
 *  runtime (in contrast to pointers that point into already allocated memory).
 *  This information is especially helpful if one needs to know which pointers
 *  need to be freed (cf. checkpoint.c).
 */
 
#ifndef __STRUCTS__

#define __STRUCTS__
#ifdef _MSC_VER
#include <wchar.h>
#endif
/*
  	#[ sav&store :
*/

/**
 *
 */

typedef struct PoSiTiOn {
	off_t p1;
} POSITION;

/**
 *
 */
 
#define INFILEINDX 15

/**
 *
 */

typedef struct {
	POSITION	nameposition;	/* Position of the name of the expression */
	POSITION	varposition;	/* Position of the list with variables */
	POSITION	varlength;		/* Length of the variable list */
	POSITION	exprposition;	/* Position of the expression itself */
/*	POSITION	exprlength;		   Length of the expression itself */
} INDXENTRY;

/**
 *
 */

typedef struct {
	POSITION	nextindex;			/* Position of next FILEINDX if any */
	LONG		number;				/* Number of used entries in this index */
	LONG		reserved;			/* For the future, and for padding */
	INDXENTRY	entries[INFILEINDX];
} FILEINDX;

/*	Next are the index structs for stored and saved expressions */

/**
 *  Defines the structure of the file header for store-files and save-files.
 *
 *  The first 8 bytes serve as a unique mark to identity save-files that
 *  contain such a header. Older versions of FORM don't have this header and
 *  will write the POSITION of the next file index (struct FiLeInDeX) here,
 *  which is always different from this pattern.
 *
 *  It is always 512 bytes long.
 */
typedef struct {
	UBYTE headermark[8];  /**< Pattern for header identification. Old versions
	                           of FORM have a maximum sizeof(POSITION) of 8 */
	UBYTE lenWORD;        /**< Number of bytes for WORD */
	UBYTE lenLONG;        /**< Number of bytes for LONG */
	UBYTE lenPOS;         /**< Number of bytes for POSITION */
	UBYTE lenPOINTER;     /**< Number of bytes for void * */
	UBYTE endianness[16]; /**< Used to determine endianness, sizeof(int) should be <= 16 */
	UBYTE sSym;           /**< sizeof(struct SyMbOl)   */
	UBYTE sInd;           /**< sizeof(struct InDeX)    */
	UBYTE sVec;           /**< sizeof(struct VeCtOr)   */
	UBYTE sFun;           /**< sizeof(struct FuNcTiOn) */
	UBYTE maxpower[16];   /**< Maximum power, see #MAXPOWER */
	UBYTE wildoffset[16]; /**< #WILDOFFSET macro         */
	UBYTE revision;       /**< Revision number of save-file system  */
	UBYTE reserved[512-8-4-16-4-16-16-1]; /**< Padding to 512 bytes */
} STOREHEADER;

/**
 *  Defines the structure of an entry in a file index (see struct FiLeInDeX).
 *  
 *  It represents one expression in the file.
 */
typedef struct InDeXeNtRy {
	POSITION	position;		/**< Position of the expression itself */
	POSITION	length;			/**< Length of the expression itself */
	POSITION	variables;		/**< Position of the list with variables */
	LONG	CompressSize;		/**< Size of buffer before compress */
	WORD	nsymbols;			/**< Number of symbols in the list */
	WORD	nindices;			/**< Number of indices in the list */
	WORD	nvectors;			/**< Number of vectors in the list */
	WORD	nfunctions;			/**< Number of functions in the list */
	WORD    size;				/**< Size of variables field */
	SBYTE	name[MAXENAME+1];	/**< Name of expression */
	PADLONG(1,5,MAXENAME+1);
} INDEXENTRY;

/**
 *  Maximum number of entries (struct InDeXeNtRy) in a file index (struct
 *  FiLeInDeX). Number is calculated such that the size of a file index is no
 *  more than 512 bytes.
 */
#define INFILEINDEX ((512-sizeof(LONG)-sizeof(POSITION))/sizeof(INDEXENTRY))
/**
 *  Number of empty filling bytes for a file index (struct FiLeInDeX). It is
 *  calculated such that the size of a file index is always 512 bytes.
 */
#define EMPTYININDEX (512-sizeof(LONG)-sizeof(POSITION)-INFILEINDEX*sizeof(INDEXENTRY))

/**
 *  Defines the structure of a file index in store-files and save-files.
 *  
 *  It contains several entries (see struct InDeXeNtRy) up to a maximum of
 *  #INFILEINDEX.
 *
 *  It is always 512 bytes long.
 */
typedef struct FiLeInDeX {
	POSITION	next;			/**< Position of next FILEINDEX if any */
	LONG	number;				/**< Number of used entries in this index */
	INDEXENTRY expression[INFILEINDEX]; /**< File index entries */
	SBYTE	empty[EMPTYININDEX];		/**< Padding to 512 bytes */
} FILEINDEX;

/**
 *
 */

typedef struct FiLeDaTa {
	FILEINDEX Index;
	POSITION Fill;
	POSITION Position;
	WORD Handle;
	WORD dirtyflag;
	PADLONG(0,2,0);
} FILEDATA;

/**
 *
 *  Contains the pointers to an array in which a binary search will be
 *  performed.
 */

typedef struct VaRrEnUm {
	WORD  *start;  /**< Start point for search. Points inbetween lo and hi */
	WORD  *lo;     /**< Start of memory area */
	WORD  *hi;     /**< End of memory area */
} VARRENUM;

/**
 *
 *  Only symb.lo gets dynamically allocated. All other pointers points into this
 *  memory.
 */

typedef struct ReNuMbEr {
	/* First stage renumbering */
	VARRENUM   symb;          /**< Symbols */
	VARRENUM   indi;          /**< Indices */
	VARRENUM   vect;          /**< Vectors */
	VARRENUM   func;          /**< Functions */
	/* Second stage renumbering */
	WORD       *symnum;       /**< Renumbered symbols */
	WORD       *indnum;       /**< Renumbered indices */
	WORD       *vecnum;       /**< Renumbered vectors */
	WORD       *funnum;       /**< Renumbered functions */
	POSITION   startposition;
	PADPOINTER(0,0,0,0);
} *RENUMBER;

/**
 *
 */

typedef struct {
	WORD	nsymbols;			/* Number of symbols in the list */
	WORD	nindices;			/* Number of indices in the list */
	WORD	nvectors;			/* Number of vectors in the list */
	WORD	nfunctions;			/* Number of functions in the list */
} VARINFO;

/*
  	#] sav&store : 
  	#[ Variables :
*/

/**
 *  Much information is stored in arrays of which we can double the size
 *  if the array proves to be too small. Such arrays are controled by
 *  a variable of type #LIST. The routines that expand the lists are in the
 *  file tools.c
 */

typedef struct {
	void *lijst;       /**< [D] Holds space for "maxnum" elements of size "size" each */
	char *message;     /**< Text for Malloc1 when allocating lijst. Set to constant string. */
	int num;           /**< Number of elements in lijst. */
	int maxnum;        /**< Maximum number of elements in lijst. */
	int size;          /**< Size of one element in lijst. */
	int numglobal;     /**< Marker for position when .global is executed. */
	int numtemp;       /**< At the moment only needed for sets and setstore. */
	int numclear;      /**< Only for the clear instruction. */
	PADPOINTER(0,6,0,0);
} LIST;

/**
 *	The KEYWORD struct defines names of commands/statements and the routine
 *	to be called when they are encountered by the compiler or preprocessor.
 */

typedef struct {
	char *name;
	TFUN func;
	int type;
	int flags;
} KEYWORD;

/**
 *	The names of variables are kept in an array. Elements of type NAMELIST
 *	point to the names and tell us what these names corresponds to in
 *	internal notation (type of the variable and its number)
 */

typedef struct NameLi {         /* For fast namesearching */
    WORD where;
    WORD type;
    WORD number;
} NAMELIST;

/**
 *  The names of variables are kept in an array. Elements of type #NAMENODE
 *  define a tree (that is kept balanced) that make it easy and fast to look for
 *  variables. See also #NAMETREE.
 */

typedef struct NaMeNode {
	LONG name;      /**< Offset into NAMETREE::namebuffer. */
	WORD parent;    /**< =-1 if no parent. */
	WORD left;      /**< =-1 if no child. */
	WORD right;     /**< =-1 if no child. */
	WORD balance;   /**< Used for the balancing of the tree. */
	WORD type;      /**< Type associated with the name. See @ref CompilerTypes "compiler types". */
	WORD number;    /**< Number of variable in #LIST's like for example C_const::SymbolList. */
	PADLONG(0,6,0);
} NAMENODE;

/**
 *  A struct of type #NAMETREE controls a complete (balanced) tree of names
 *  for the compiler. The compiler maintains several of such trees and the
 *  system has been set up in such a way that one could define more of them
 *  if we ever want to work with local name spaces.
 */

typedef struct NaMeTree {
	NAMENODE *namenode;      /**< [D] Vector of #NAMENODE's. Number of elements is #nodesize.
	                              =0 if no memory has been allocated. */
	LONG     nodesize;       /**< Maximum number of elements in #namenode. */
	LONG     nodefill;       /**< Number of currently used nodes in #namenode. */
	UBYTE    *namebuffer;    /**< [D] Buffer that holds all the name strings refered to by the
	                              NAMENODE's. Allocation size is #namesize. =0 if no memory
	                              has been allocated. */
	LONG     namesize;       /**< Allocation size of #namebuffer in bytes. */
	LONG     namefill;       /**< Number of bytes occupied. */
	LONG     oldnamefill;    /**< UNUSED */
	LONG     oldnodefill;    /**< UNUSED */
	LONG     globalnamefill; /**< Set by .global statement to the value of #namefill. When a .store
	                              command is processed, this value will be used to reset namefill.*/
	LONG     globalnodefill; /**< Same usage as #globalnamefill, but for nodefill. */
	LONG     clearnamefill;  /**< Marks the reset point used by the .clear statement. */
	LONG     clearnodefill;  /**< Marks the reset point used by the .clear statement. */
	WORD     headnode;       /**< Offset in #namenode of head node. =-1 if tree is empty. */
	PADPOINTER(10,0,1,0);
} NAMETREE;

/**
 *	The subexpressions in the compiler are kept track of in a (balanced) tree
 *	to reduce the need for subexpressions and hence save much space in
 *	large rhs expressions (like when we have xxxxxxx occurrences of objects
 *	like f(x+1,x+1) in which each x+1 becomes a subexpression.
 *	The struct that controls this tree is COMPTREE.
 */

typedef struct tree {
	int parent;  /**< Index of parent */
	int left;    /**< Left child (if not -1) */
	int right;   /**< Right child (if not -1) */
	int value;   /**< The object to be sorted and searched */
	int blnce;   /**< Balance factor */
} COMPTREE;

/**
 *
 */
 
typedef struct MiNmAx {
	WORD mini;          /**< Minimum value */
	WORD maxi;          /**< Maximum value */
	WORD size;          /**< Value of one unit in this position. */
} MINMAX;

/**
 *
 */
 
typedef struct BrAcKeTiNdEx {	/* For indexing brackets in local expressions */
	POSITION start;				/* Place where bracket starts - start of expr */
	POSITION next;				/* Place of next indexed bracket in expr */
	LONG bracket;				/* Offset of position in bracketbuffer */
	PADPOINTER(1,0,0,0);
} BRACKETINDEX;

/**
 *
 */

typedef struct BrAcKeTiNfO {
	BRACKETINDEX *indexbuffer; /**< [D] */
	WORD  *bracketbuffer;      /**< [D] */
	LONG  bracketbuffersize;
	LONG  indexbuffersize;
	LONG  bracketfill;
	LONG  indexfill;
	WORD  SortType;            /**< The sorting criterium used (like POWERFIRST etc) */
	PADPOINTER(4,0,1,0);
} BRACKETINFO;

/**
 *
 *  buffers, mm, flags, and prototype are always dynamically allocated,
 *  tablepointers only if needed (=0 if unallocated),
 *  boomlijst and argtail only for sparse tables.
 *
 *  Allocation is done for both the normal and the stub instance (spare),
 *  except for prototype and argtail which share memory.
 */

typedef struct TaBlEs {
	WORD    *tablepointers; /**< [D] Start in tablepointers table. */
#ifdef WITHPTHREADS
	WORD    **prototype;    /**< [D] The wildcard prototyping for arguments */
	WORD    **pattern;      /**< The pattern with which to match the arguments */
#else
	WORD    *prototype;     /**< [D] The wildcard prototyping for arguments */
	WORD    *pattern;       /**< The pattern with which to match the arguments */
#endif
	int     prototypeSize;  /**< Size of allocated memory for prototype in bytes. */
	MINMAX  *mm;            /**< [D] Array bounds, dimension by dimension. # elements = numind. */
	WORD    *flags;         /**< [D] Is element in use ? etc. # elements = numind. */
	COMPTREE *boomlijst;    /**< [D] Tree for searching in sparse tables */
	UBYTE   *argtail;       /**< [D] The arguments in characters. Starts for tablebase
	                             with parenthesis to indicate tail */
	struct TaBlEs *spare;   /**< [D] For tablebase. Alternatingly stubs and real */
	WORD    *buffers;       /**< [D] When we use more than one compiler buffer. */
	LONG    totind;         /**< Total number requested */
	LONG    reserved;       /**< Total reservation in tablepointers for sparse */
	LONG    defined;        /**< Number of table elements that are defined */
	LONG    mdefined;       /**< Same as defined but after .global */
	int     numind;         /**< Number of array indices */
	int     bounds;         /**< Array bounds check on/off. */
	int     strict;         /**< >0: all must be defined. <0: undefined not substitute */
	int     sparse;         /**< > 0 --> sparse table */
	int     numtree;        /**< For the tree for sparse tables */
	int     rootnum;        /**< For the tree for sparse tables */
	int     MaxTreeSize;    /**< For the tree for sparse tables */
	WORD    bufnum;         /**< Each table potentially its own buffer */
	WORD    bufferssize;    /**< When we use more than one compiler buffer */
	WORD    buffersfill;    /**< When we use more than one compiler buffer */
	WORD    tablenum;       /**< For testing of tableuse */
	WORD    mode;           /**< 0: normal, 1: stub */
	WORD    numdummies;     /**<  */
	PADPOINTER(4,7,6,0);
} *TABLES;

/**
 *
 */

typedef struct ExPrEsSiOn {
	RENUMBER renum;			/* For Renumbering of global stored expressions */
	BRACKETINFO *bracketinfo;
	BRACKETINFO *newbracketinfo;
	WORD	*renumlists;    /**< Allocated only for threaded version if variables exist,
	                             else points to AN.dummyrenumlist */
	WORD	*inmem;			/* If in memory like e.g. a polynomial */
	POSITION	onfile;
	POSITION	prototype;
	LONG	counter;
	LONG	name;
	WORD	hidelevel;
	WORD	vflags;			/* Various flags */
	WORD	printflag;
	WORD	status;
	WORD	replace;
	WORD	node;
	WORD	whichbuffer;
	WORD	namesize;
	WORD	compression;
	WORD	numdummies;
#ifdef WITHPTHREADS
	WORD	partodo;		/* Whether to be done in parallel mode */
	PADPOINTER(2,0,11,0);
#else
	PADPOINTER(2,0,10,0);
#endif
} *EXPRESSIONS;

/**
 *
 */

typedef struct SyMbOl {			/* Don't change unless altering .sav too */
	LONG	name;				/* Location in names buffer */
	WORD	minpower;			/* Minimum power admissible */
	WORD	maxpower;			/* Maximum power admissible */
	WORD	complex;			/* Properties wrt complex conjugation */
	WORD	number;				/* Number when stored in file */
	WORD	flags;				/* Used to indicate usage when storing */
	WORD	node;
	WORD	namesize;
	PADLONG(0,7,0);
} *SYMBOLS;

/**
 *
 */

typedef struct InDeX {			/* Don't change unless altering .sav too */
	LONG	name;				/* Location in names buffer */
	WORD	type;				/* Regular or dummy */
	WORD	dimension;			/* Value of d_(n,n) or -number of symbol */
	WORD	number;				/* Number when stored in file */
	WORD	flags;				/* Used to indicate usage when storing */
	WORD    nmin4;				/* Used for n-4 if dimension < 0 */
	WORD	node;
	WORD	namesize;
	PADLONG(0,7,0);
} *INDICES;

/**
 *
 */

typedef struct VeCtOr {			/* Don't change unless altering .sav too */
	LONG	name;				/* Location in names buffer */
	WORD	complex;			/* Properties under complex conjugation */
	WORD	number;				/* Number when stored in file */
	WORD	flags;				/* Used to indicate usage when storing */
	WORD	node;
	WORD	namesize;
	PADLONG(0,5,0);
} *VECTORS;

/**
 *  Contains all information about a function. Also used for tables.
 *  It is used in the #LIST elements of #AC.
 */

typedef struct FuNcTiOn {  /* Don't change unless altering .sav too */
	TABLES  tabl;          /**< Used if redefined as table. != 0 if function is a table */
	LONG    symminfo;      /**< Info regarding symm properties offset in buffer */
	LONG    name;          /**< Location in namebuffer of #NAMETREE */
	WORD    commute;       /**< Commutation properties */
	WORD    complex;       /**< Properties under complex conjugation */
	WORD    number;        /**< Number when stored in file */
	WORD    flags;         /**< Used to indicate usage when storing */
	WORD    spec;          /**< Regular, Tensor, etc. See @ref FunSpecs. */
	WORD    symmetric;     /**< > 0 if symmetric properties */
	WORD    node;          /**< Location in namenode of #NAMETREE */
	WORD    namesize;      /**< Length of the name */
	PADPOINTER(2,0,8,0);
} *FUNCTIONS;

/**
 *
 */

typedef struct SeTs {
	LONG	name;				/* Location in names buffer */
	WORD	type;				/* Symbol, vector, index or function */
	WORD	first;				/* First element in setstore */
	WORD	last;				/* Last element in setstore */
	WORD	node;
	WORD	namesize;
	PADLONG(1,5,0);
} *SETS;

/**
 *
 */

typedef struct DuBiOuS {		/* Undeclared objects. Just for compiler. */
	LONG	name;				/* Location in names buffer */
	WORD	node;
	WORD	dummy;
	PADLONG(0,2,0);
} *DUBIOUSV;

typedef struct DoLlArS {
	WORD	*where;				/* A pointer(!) to the object */
	LONG	size;				/* The number of words */
	LONG	name;
#ifdef WITHPTHREADS
	pthread_mutex_t	pthreadslockread;
	pthread_mutex_t	pthreadslockwrite;
#endif
	WORD	type;
	WORD	node;
	WORD	index;
	WORD	zero;
	WORD	numdummies;
	WORD	reserved;
	PADPOINTER(2,0,6,0);
} *DOLLARS;

/**
 *
 */

typedef struct MoDoPtDoLlArS {
	WORD	number;
	WORD	type;
#ifdef WITHPTHREADS
	DOLLARS	dstruct;	/* If local dollar: list of DOLLARS for each thread */
#endif
} MODOPTDOLLAR;

/**
 *
 */

typedef struct fixedset {
	char *name;
	char *description;
	int type;
} FIXEDSET;

/**
 *
 */

typedef struct TaBlEbAsEsUbInDeX {
	POSITION where;
	LONG size;
} TABLEBASESUBINDEX;

/**
 *
 */

typedef struct TaBlEbAsE {
	UBYTE *name;
	int *tablenumbers;		/* Number of each table */
	TABLEBASESUBINDEX *subindex;		/* For each table */
	POSITION fillpoint;
	POSITION current;
	int numtables;
} TABLEBASE;

/**
 *	The struct FUN_INFO is used for information about functions in the file
 *	smart.c which is supposed to intelligently look for patterns in
 *	complicated wildcard situations involving symmetric functions.
 */

typedef struct {
	WORD *location;
	int numargs;
	int numfunnies;
	int numwildcards;
	int symmet;
	int tensor;
	int commute;
} FUN_INFO;
 
/*
  	#] Variables : 
  	#[ Files :
*/

/**
 *	The type FILEHANDLE is the struct that controls all relevant information
 *	of a file, whether it is open or not. The file may even not yet exist.
 *	There is a system of caches (PObuffer) and as long as the information
 *	to be written still fits inside the cache the file may never be
 *	created. There are variables that can store information about different
 *	types of files, like scratch files or sort files.
 *	Depending on what is available in the system we may also have information
 *	about gzip compression (currently sort file only) or locks (TFORM).
 */

typedef struct FiLe {
    WORD *PObuffer;             /* Address of the intermediate buffer */
    WORD *POstop;               /* End of the buffer */
    WORD *POfill;               /* Fill position of the buffer */
    WORD *POfull;               /* Full buffer when only cached */
#ifdef WITHPTHREADS
    WORD *wPObuffer;             /* Address of the intermediate worker buffer */
    WORD *wPOstop;               /* End of the worker buffer */
    WORD *wPOfill;               /* Fill position of the worker buffer */
    WORD *wPOfull;               /* Full buffer when only worker cached */
#endif
	char *name;					/* name of the file */
#ifdef WITHZLIB
	z_streamp zsp;				/* The pointer to the stream struct for gzip */
	Bytef *ziobuffer;			/* The output buffer for compression */
#endif
	POSITION POposition;    	/* File position */
    POSITION filesize;          /* Because SEEK_END is unsafe on IBM */
	ULONG numblocks;			/* Number of blocks in file */
	ULONG inbuffer;				/* Block in the buffer */
    LONG POsize;                /* size of the buffer */
#ifdef WITHZLIB
    LONG ziosize;               /* size of the zoutbuffer */
#endif
#ifdef WITHPTHREADS
    LONG wPOsize;                /* size of the worker buffer */
	pthread_mutex_t	pthreadslock;
#endif
    int handle;					/**< Our own handle. Equal -1 if no file exists. */
	int active;					/* File is open or closed. Not used. */
#ifdef WITHZLIB
	PADPOINTER(4,2,1,0);
#else
	PADPOINTER(3,2,1,0);
#endif
} FILEHANDLE;

/**
 *	Input is read from 'streams' which are represented by objects of type
 *	STREAM. A stream can be a file, a do-loop, a procedure, the string value
 *	of a preprocessor variable .....
 *	When a new stream is opened we have to keep information about where
 *	to fall back in the parent stream to allow this to happen even in the
 *	middle of reading names etc as would be the case with a`i'b
 */
 
typedef struct StreaM {
	UBYTE *buffer;        /**< [D] Size in buffersize */
	UBYTE *pointer;       /**< pointer into buffer memory */
	UBYTE *top;           /**< pointer into buffer memory */
	UBYTE *FoldName;      /**< [D] */
	UBYTE *name;          /**< [D] */
	UBYTE *pname;         /**< for DOLLARSTREAM and PREVARSTREAM it points always to name, else it
	                           is undefined */
	off_t fileposition;
	off_t linenumber;
	off_t prevline;
	long buffersize;
	long bufferposition;
	long inbuffer;
	int previous;
	int handle;
	int type;
	int prevars;
	int previousNoShowInput;
	int eqnum;
	int afterwards;
	int olddelay;
	int oldnoshowinput;
	UBYTE isnextchar;
	UBYTE nextchar[2];
	UBYTE reserved;
	PADPOINTER(3,8,0,4);
} STREAM;

/*
  	#] Files : 
  	#[ Traces :
*/

/**
 *	The struct TRACES keeps track of the progress during the expansion
 *	of a 4-dimensional trace. Each time a term gets generated the expansion
 *	tree continues in the next statement. When it returns it has to know
 *	where to continue. The 4-dimensional traces are more complicated
 *	than the n-dimensional traces (see TRACEN) because of the extra tricks
 *	that can be used. They are responsible for the shorter final expressions.
 */

typedef struct TrAcEs {			/* For computing 4 dimensional traces */
	WORD		*accu;		/* NUMBER * 2 */
	WORD		*accup;
	WORD		*termp;
	WORD		*perm;		/* number */
	WORD		*inlist;	/* number */
	WORD		*nt3;		/* number/2 */
	WORD		*nt4;		/* number/2 */
	WORD		*j3;		/* number*2 */
	WORD		*j4;		/* number*2 */
	WORD		*e3;		/* number*2 */
	WORD		*e4;		/* number */
	WORD		*eers;		/* number/2 */
	WORD		*mepf;		/* number/2 */
	WORD		*mdel;		/* number/2 */
	WORD		*pepf;		/* number*2 */
	WORD		*pdel;		/* number*3/2 */
	WORD		sgn;
	WORD		stap;
	WORD		step1,kstep,mdum;
	WORD		gamm,ad,a3,a4,lc3,lc4;
	WORD		sign1,sign2,gamma5,num,level,factor,allsign;
	WORD		finalstep;
	PADPOINTER(0,0,19,0);
} TRACES;

/**
 *	The struct TRACEN keeps track of the progress during the expansion
 *	of a 4-dimensional trace. Each time a term gets generated the expansion
 *	tree continues in the next statement. When it returns it has to know
 *	where to continue.
 */

typedef struct TrAcEn {			/* For computing n dimensional traces */
	WORD		*accu;		/* NUMBER */
	WORD		*accup;
	WORD		*termp;
	WORD		*perm;		/* number */
	WORD		*inlist;	/* number */
	WORD		sgn,num,level,factor,allsign;
	WORD		allignment;
	PADPOINTER(0,0,6,0);
} *TRACEN;

/*
  	#] Traces : 
  	#[ Preprocessor :
*/

/**
 *	An element of the type PREVAR is needed for each preprocessor variable.
 */
 
typedef struct pReVaR {
	UBYTE *name;		/**< allocated */
	UBYTE *value;		/**< points into memory of name */
	UBYTE *argnames;	/**< names of arguments, zero separated. points into memory of name */
	int nargs;			/**< 0 = regular, >= 1: number of macro arguments. total number */
	int wildarg;		/**< The number of a potential ?var. If none: 0. wildarg<nargs */
	PADPOINTER(0,2,0,0);
} PREVAR;

/**
 *	Used by the preprocessor to load the contents of a doloop or a procedure.
 *	The struct PRELOAD is used both in the DOLOOP and PROCEDURE structs.
 */

typedef struct {
	UBYTE	*buffer;
    LONG	size;
	PADPOINTER(1,0,0,0);
} PRELOAD;

/**
 *	An element of the type PROCEDURE is needed for each procedure in the system.
 */

typedef struct {
	PRELOAD p;
	UBYTE	*name;
	int		loadmode;
	PADPOINTER(0,1,0,0);
} PROCEDURE;

/**
 *	Each preprocessor do loop has a struct of type DOLOOP to keep track
 *	of all relevant parameters like where the beginning of the loop is,
 *	what the boundaries, increment and value of the loop parameter are, etc.
 *	Also we keep the whole loop inside a buffer of type PRELOAD
 */

typedef struct DoLoOp {
	PRELOAD p;          /**< size, name and buffer */
	UBYTE *name;        /**< pointer into PRELOAD buffer */
	UBYTE *vars;        /* for {} or name of expression */
	UBYTE *contents;
	UBYTE *dollarname;  /**< For loop over terms in expression. Allocated with Malloc1() */
	LONG startlinenumber;
	LONG firstnum;
	LONG lastnum;
	LONG incnum;
	int type;
	int NoShowInput;
	int errorsinloop;
	int firstloopcall;
	PADPOINTER(4,4,0,0);
} DOLOOP;

/**
 *	The struct bit_field is used by set_in, set_set, set_del and set_sub.
 *	They in turn are used in pre.c to toggle bits that indicate whether
 *	a character can be used as a separator of function arguments.
 *	This facility is used in the communication with external channels.
 */

struct  bit_field {	/* Assume 8 bits per byte */
    UINT bit_0        : 1;
    UINT bit_1        : 1;
    UINT bit_2        : 1;
    UINT bit_3        : 1;
    UINT bit_4        : 1;
    UINT bit_5        : 1;
    UINT bit_6        : 1;
    UINT bit_7        : 1;
};

/**
 *	Used in set_in, set_set, set_del and set_sub.
 */

typedef struct bit_field set_of_char[32];

/**
 *	Used in set_in, set_set, set_del and set_sub.
 */

typedef struct bit_field *one_byte;

/**
 *	The struct HANDLERS is used in the communication with external channels.
 */

typedef struct {
	WORD newlogonly;
	WORD newhandle;
	WORD oldhandle;
	WORD oldlogonly;
	WORD oldprinttype;
	WORD oldsilent;
} HANDLERS;

/*
  	#] Preprocessor : 
  	#[ Varia :
*/

/**
 *	The CBUF struct is used by the compiler. It is a compiler buffer of which
 *	since version 3.0 there can be many.
 */

typedef struct CbUf {
	WORD *Buffer;         /**< [D] Size in BufferSize */
	WORD *Top;            /**< pointer to the end of the Buffer memory */
	WORD *Pointer;        /**< pointer into the Buffer memory */
	WORD **lhs;           /**< [D] Size in maxlhs. list of pointers into Buffer. */
	WORD **rhs;           /**< [D] Size in maxrhs. list of pointers into Buffer. */
	LONG *CanCommu;       /**< points into rhs memory behind WORD* area. */
	LONG *NumTerms;       /**< points into rhs memory behind CanCommu area */
	WORD *numdum;         /**< points into rhs memory behind NumTerms */
	COMPTREE *boomlijst;  /**< [D] Number elements in MaxTreeSize */
	LONG BufferSize;      /**< Number of allocated WORD's in Buffer */
	int numlhs;
	int numrhs;
	int maxlhs;
	int maxrhs;
	int mnumlhs;
	int mnumrhs;
	int numtree;
	int rootnum;
	int MaxTreeSize;
	PADPOINTER(1,9,0,0);
} CBUF;

/**
 *	When we read input from text files we have to remember not only their
 *	handle but also their name. This is needed for error messages.
 *	Hence we call such a file a channel and reserve a struct of type
 *	#CHANNEL to allow to lay this link.
 */

typedef struct ChAnNeL {
	char *name;          /**< [D] Name of the channel */
	int handle;          /**< File handle */
	PADPOINTER(0,1,0,0);
} CHANNEL;

/**
 *	Each setup parameter has one element of the struct SETUPPARAMETERS
 *	assigned to it. By binary search in the array of them we can then
 *	locate the proper element by name.
 */
 
typedef struct {
	UBYTE *parameter;
	int type;
	int flags;
	long value;
} SETUPPARAMETERS;

/**
 *	The NESTING struct is used when we enter the argument of functions and
 *	there is the possibility that we have to change something there.
 *	Because functions can be nested we have to keep track of all levels
 *	of functions in case we have to move the outer layers to make room
 *	for a larger function argument.
 */

typedef struct NeStInG {
	WORD *termsize;
	WORD *funsize;
	WORD *argsize;
} *NESTING;

/**
 *	The struct of type STORECACHE is used by a caching system for reading
 *	terms from stored expressions. Each thread should have its own system
 *	of caches.
 */

typedef struct StOrEcAcHe {
	struct StOrEcAcHe *next;
	POSITION position;
	POSITION toppos;
	WORD buffer[2];
	PADPOINTER(2,0,2,0);
} *STORECACHE;

/**
 *	The struct PERM is used to generate all permutations when the pattern
 *	matcher has to try to match (anti)symmetric functions.
 */

typedef struct PeRmUtE {
	WORD *objects;
	WORD sign;
	WORD n;
	WORD cycle[MAXMATCH];
	PADPOINTER(0,0,MAXMATCH+2,0);
} PERM;

/**
 *	The struct DISTRIBUTE is used to help the pattern
 *	matcher when matching antisymmetric tensors.
 */

typedef struct DiStRiBuTe {
	WORD *obj1;
	WORD *obj2;
	WORD *out;
	WORD sign;
	WORD n1;
	WORD n2;
	WORD n;
	WORD cycle[MAXMATCH];
} DISTRIBUTE;

/**
 *	The struct SORTING is used to control a sort operation.
 *	It includes a small and a large buffer and arrays for keeping track
 *	of various stages of the (merge) sorts.
 *	Each sort level has its own struct and different levels can have
 *	different sizes for its arrays.
 *	Also different threads have their own set of SORTING structs.
 */

typedef struct sOrT {
	WORD *lBuffer;				/* The large buffer */
	WORD *lTop;					/* End of the large buffer */
	WORD *lFill;				/* The filling point of the large buffer */
	WORD *used;					/*  auxiliary during actual sort */
	WORD *sBuffer;				/* The small buffer */
	WORD *sTop;					/* End of the small buffer */
	WORD *sTop2;				/* End of the extension of the small buffer */
	WORD *sHalf;				/* Halfway point in the extension */
	WORD *sFill;				/* Filling point in the small buffer */
	WORD **sPointer;			/* Pointers to terms in the small buffer */
	WORD **PoinFill;			/* Filling point for pointers to the sm.buf */
	WORD **SplitScratch;		/* Excess pointer space for the merge sort */
	WORD *cBuffer;				/* Compress buffer (if it exists) */
	WORD **Patches;				/* Positions of patches in large buffer */
	WORD **pStop;				/* Ends of patches in the large buffer */
	WORD **poina;				/*  auxiliary during actual sort */
	WORD **poin2a;				/*  auxiliary during actual sort */
	WORD *ktoi;					/*  auxiliary during actual sort */
	WORD *tree;					/*  auxiliary during actual sort */
#ifdef WITHZLIB
	WORD *fpcompressed;			/*  is output filepatch compressed? */
	WORD *fpincompressed;		/*  is input filepatch compressed? */
	z_streamp zsparray;			/*  the array of zstreams for decompression */
#endif
	POSITION *fPatches;			/* Positions of output file patches */
	POSITION *inPatches;		/* Positions of input file patches */
	POSITION *fPatchesStop;		/* Positions of output file patches */
	POSITION *iPatches;			/* Input file patches, Points to fPatches or inPatches */
	FILEHANDLE *f;				/* The actual output file */
	FILEHANDLE file;			/* The own sort file */
	FILEHANDLE **ff;			/* Handles for a staged sort */
	LONG sTerms;				/* Terms in small buffer */
	LONG LargeSize;				/* Size of large buffer (in words) */
	LONG SmallSize;				/* Size of small buffer (in words) */
	LONG SmallEsize;			/* Size of small + extension (in words) */
	LONG TermsInSmall;			/* Maximum number of terms in small buffer */
	LONG Terms2InSmall;			/* with extension for polyfuns etc. */
	LONG GenTerms;				/* Number of generated terms */
	LONG TermsLeft;				/* Number of terms still in existence */
	LONG GenSpace;				/* Amount of space of generated terms */
	LONG SpaceLeft;				/* Space needed for still existing terms */
	LONG putinsize;				/* Size of buffer in putin */
	LONG ninterms;				/* Which input term ? */
	POSITION SizeInFile[3];		/* Sizes in the various files */
	int MaxPatches;				/* Maximum number of patches in large buffer */
	int MaxFpatches;			/* Maximum number of patches in one filesort */
	int type;					/* Main, function or sub(routine) */
	int lPatch;					/* Number of patches in the large buffer */
	int fPatchN1;				/* Number of patches in input file */
	int PolyWise;				/* Is there a polyfun and if so, where? */
	int PolyFlag;				/*  */
	int cBufferSize;			/* Size of the compress buffer */
	int maxtermsize;			/* Keeps track for buffer allocations */
	int newmaxtermsize;			/* Auxiliary for maxtermsize */
	int outputmode;				/* Tells where the output is going */
	int stagelevel;				/* In case we have a 'staged' sort */
	WORD fPatchN;				/* Number of patches on file (output) */
	WORD inNum;					/* Number of patches on file (input) */
	WORD stage4;				/* Are we using stage4? */
	PADPOINTER(15,12,3,0);
} SORTING;

#ifdef WITHPTHREADS

/**
 *	The SORTBLOCK's are used by TFORM when the master has to merge the sorted
 *	results of each of the workers.
 */

typedef struct SoRtBlOcK {
    pthread_mutex_t *MasterBlockLock;
    WORD    **MasterStart;
    WORD    **MasterFill;
    WORD    **MasterStop;
    int     MasterNumBlocks;
    int     MasterBlock;
    int     FillBlock;
} SORTBLOCK;
#endif

#ifdef DEBUGGER
typedef struct DeBuGgInG {
    int eflag;
    int printflag;
    int logfileflag;
    int stdoutflag;
    PADPOINTER(0,4,0,0);
} DEBUGSTR;
#endif

#ifdef WITHPTHREADS

/**
 *	The THREADBUCKET struct defines one of the buckets used to pass terms
 *	from the master to the workers in TFORM.
 */

typedef struct ThReAdBuCkEt {
	POSITION *deferbuffer;      /* For Keep Brackets: remember position */
	WORD *threadbuffer;         /* Here are the (primary) terms */
	WORD *compressbuffer;       /* For keep brackets we need the compressbuffer */
	LONG threadbuffersize;      /* Number of words in threadbuffer */
	LONG ddterms;               /* Number of primary+secondary terms represented */
	LONG firstterm;				/* The number of the first term in the bucket */
	pthread_mutex_t lock;       /* For the load balancing phase */
	int  free;                  /* Status of the bucket */
	int  totnum;                /* Total number of primary terms */
	int  usenum;                /* Which is the term being used at the moment */
	int  busy;                  /*  */
} THREADBUCKET;

#endif

/**
 *	The POLYMOD struct controls one univariate polynomial of which the
 *	coefficients have been taken modulus a (prime) number that fits inside
 *	a variable of type WORD. The polynomial is stored as an array of
 *	coefficients of size WORD.
 */

typedef struct {
	WORD	*coefs;				/* The array of coefficients */
	WORD	numsym;				/* The number of the symbol in the polynomial */
	WORD	arraysize;			/* The size of the allocation of coefs */
	WORD	polysize;			/* The maximum power in the polynomial */
	WORD	modnum;				/* The prime number of the modulus */
} POLYMOD;

/**
 *	The POLYPADIC struct controls one univariate polynomial of which the
 *	coefficients are in p-adic notation rather than notation modulus FULLMAX.
 *	The terms are stored in sparse notation as regular terms that have
 *	a subterm of type SYMBOL. The difference is the coefficient.
 *	The coefficient is an integer expanded in powers of a prime which is
 *	given in the element modnum (the prime has to fit inside a variable
 *	of type WORD).
 */

typedef struct {
	POLYMOD	*ppoly;				/* An array of polynomials */
	WORD	*ncoefs;			/* How many powers of the prime in each coef */
	WORD	numsym;				/* The number of the symbol in the polynomial */
	WORD	padicsize;			/* Elements allocated in ppoly; */
	WORD	numpow;				/* Maximum power of modnum actually used */
	WORD	arraysize;			/* The size of the allocation of coefs */
	WORD	polysize;			/* The maximum power in the polynomial */
	WORD	modnum;				/* The prime number of the modulus */
} POLYPADIC;

/*
  	#] Varia : 
    #[ A :
 		#[ M : The M struct is for global settings at startup or .clear
*/

/**
 *	The M_const struct is part of the global data and resides in the #ALLGLOBALS
 *	struct #A under the name #M.
 *	We see it used with the macro #AM as in AM.S0.
 *	It contains global settings at startup or .clear.
 */

struct M_const {
    SORTING *S0;                   /**< [D] The main sort buffer */
    WORD    *gcmod;                /**< Global setting of modulus. Uses AC.cmod's memory */
    WORD    *gpowmod;              /**< Global setting printing as powers. Uses AC.cmod's memory */
    UBYTE   *TempDir;              /* (M) Path with where the temporary files go */
    UBYTE   *IncDir;               /* (M) Directory path for include files */
    UBYTE   *InputFileName;        /* (M) */
    UBYTE   *LogFileName;          /* (M) */
    UBYTE   *OutBuffer;            /* (M) Output buffer in pre.c */
    UBYTE   *Path;                 /* (M) */
    UBYTE   *SetupDir;             /* (M) Directory with setup file */
    UBYTE   *SetupFile;            /* (M) Name of setup file */
    UBYTE   *Zip1Buffer;           /* (M) First Zip compression buffer */
    UBYTE   *Zip2Buffer;           /* (M) Second Zip compression buffer */
    POSITION zeropos;              /* (M) is zero */
#ifdef WITHPTHREADS
    pthread_mutex_t handlelock;    /* (M) */
    pthread_mutex_t storefilelock; /* (M) */
    LONG    ThreadScratSize;       /* (M) Size of Fscr[0/2] buffers of the workers */
    LONG    ThreadScratOutSize;    /* (M) Size of Fscr[1] buffers of the workers */
#endif
    LONG    MaxTer;                /* (M) Maximum term size. Fixed at setup. In Bytes!!!*/
    LONG    CompressSize;          /* (M) Size of Compress buffer */
    LONG    ScratSize;             /* (M) Size of Fscr[] buffers */
    LONG    SizeStoreCache;        /* (M) Size of the chaches for reading global expr. */
    LONG    MaxStreamSize;         /* (M) Maximum buffer size in reading streams */
    LONG    SIOsize;               /* (M) Sort InputOutput buffer size */
    LONG    SLargeSize;            /* (M) */
    LONG    SSmallEsize;           /* (M) */
    LONG    SSmallSize;            /* (M) */
    LONG    STermsInSmall;         /* (M) */
    LONG    MaxBracketBufferSize;  /* (M) Max Size for B+ or AB+ per expression */
    LONG    ZipBufferSize;         /* (M) Size of buffer for gzip compression */
    LONG    hSlavePatchSize;       /* (M) */
    LONG    gSlavePatchSize;       /* (M) */
    LONG    shmWinSize;            /* (M) size for shared memory window used in communications */
    LONG    OldChildTime;          /* (M) Zero time. Needed in timer. */
    LONG    OldSecTime;            /* (M) Zero time for measuring wall clock time */
    LONG    OldMilliTime;          /* (M) Same, but milli seconds */
    LONG    WorkSize;              /* (M) Size of WorkSpace */
    LONG    gThreadBucketSize;     /* (C) */
    LONG    ggThreadBucketSize;    /* (C) */
    int     FileOnlyFlag;          /* (M) Writing only to file */
    int     Interact;              /* (M) Interactive mode flag */
    int     MaxParLevel;           /* (M) Maximum nesting of parantheses */
    int     OutBufSize;            /* (M) size of OutBuffer */
    int     SMaxFpatches;          /* (M) */
    int     SMaxPatches;           /* (M) */
    int     StdOut;                /* (M) Regular output channel */
    int     ginsidefirst;          /* (M) Not used yet */
    int     gDefDim;               /* (M) */
    int     gDefDim4;              /* (M) */
    int     NumFixedSets;          /* (M) Number of the predefined sets */
    int     NumFixedFunctions;     /* (M) Number of built in functions */
    int     rbufnum;               /* (M) startup compiler buffer */
    int     dbufnum;               /* (M) dollar variables */
    int     SkipClears;            /* (M) Number of .clear to skip at start */
    int     gfunpowers;            /* (M) */
    int     gStatsFlag;            /* (M) */
    int     gNamesFlag;            /* (M) */
    int     gCodesFlag;            /* (M) */
    int     gSortType;             /* (M) */
    int     gproperorderflag;      /* (M) */
    int     hparallelflag;         /* (M) */
    int     gparallelflag;         /* (M) */
    int     totalnumberofthreads;  /* (M) */
    int     gThreadStats;
    int     ggThreadStats;
    int     gFinalStats;
    int     ggFinalStats;
    int     gThreadsFlag;
    int     ggThreadsFlag;
    int     gThreadBalancing;
    int     ggThreadBalancing;
    int     gThreadSortFileSynch;
    int     ggThreadSortFileSynch;
    int     maxFlevels;            /* () maximum function levels */
    int     resetTimeOnClear;      /* (M) */
    int     gcNumDollars;          /* () number of dollars for .clear */
    int     MultiRun;
    int     gNoSpacesInNumbers;    /* For very long numbers */
    int     ggNoSpacesInNumbers;   /* For very long numbers */
    int     polygcdchoice;
    WORD    MaxTal;                /* (M) Maximum number of words in a number */
    WORD    IndDum;                /* (M) Basis value for dummy indices */
    WORD    DumInd;                /* (M) */
    WORD    WilInd;                /* (M) Offset for wildcard indices */
    WORD    gncmod;                /* (M) Global setting of modulus. size of gcmod */
    WORD    gnpowmod;              /* (M) Global printing as powers. size gpowmod */
    WORD    gUnitTrace;            /* (M) Global value of Tr[1] */
    WORD    gOutputMode;           /* (M) */
    WORD    gOutputSpaces;         /* (M) */
    WORD    gOutNumberType;        /* (M) */
    WORD    gUniTrace[4];          /* (M) */
    WORD    MaxWildcards;          /* (M) Maximum number of wildcards */
    WORD    mTraceDum;             /* (M) Position/Offset for generated dummies */
    WORD    OffsetIndex;           /* (M) */
    WORD    OffsetVector;          /* (M) */
    WORD    RepMax;                /* (M) Max repeat levels */
    WORD    LogType;               /* (M) Type of writing to log file */
    WORD    ggStatsFlag;           /* (M) */
    WORD    gLineLength;           /* (M) */
    WORD    qError;                /* (M) Only error checking {-c option} */
    WORD    FortranCont;           /* (M) Fortran Continuation character */
    WORD    HoldFlag;              /* (M) Exit on termination? */
    WORD    Ordering[15];          /* (M) Auxiliary for ordering wildcards */
    WORD    silent;                /* (M) Silent flag. Only results in output. */
    WORD    tracebackflag;         /* (M) For tracing errors */
    WORD    expnum;                /* (M) internal number of ^ function */
    WORD    denomnum;              /* (M) internal number of / function */
    WORD    facnum;                /* (M) internal number of fac_ function */
    WORD    invfacnum;             /* (M) internal number of invfac_ function */
    WORD    sumnum;                /* (M) internal number of sum_ function */
    WORD    sumpnum;               /* (M) internal number of sump_ function */
    WORD    OldOrderFlag;          /* (M) Flag for allowing old statement order */
    WORD    termfunnum;            /* (M) internal number of term_ function */
    WORD    matchfunnum;           /* (M) internal number of match_ function */
    WORD    countfunnum;           /* (M) internal number of count_ function */
    WORD    polyfunnum;            /* (M) internal number of poly_ function */
    WORD    polygetremnum;         /* (M) internal number of polygetrem_ function */
    WORD    polytopnum;            /* (M) internal number of maximum poly function */
    WORD    gPolyFun;              /* (M) global value of PolyFun */
    WORD    gPolyFunType;          /* (M) global value of PolyFun */
    WORD    safetyfirst;           /* (M) for testing special versions */
    WORD    dollarzero;            /* (M) for dollars with zero value */
    WORD    atstartup;             /* To protect against DATE_ ending in \n */
    WORD    exitflag;              /* (R) For the exit statement */
    WORD    NumStoreCaches;        /* () Number of storage caches per processor */
    WORD    gIndentSpace;          /* For indentation in output */
    WORD    ggIndentSpace;
    WORD    gShortStatsMax;        /**< For  On FewerStatistics 10; */
    WORD    ggShortStatsMax;       /**< For  On FewerStatistics 10; */
};
/*
 		#] M : 
 		#[ P : The P struct defines objects set by the preprocessor
*/
/**
 *	The P_const struct is part of the global data and resides in the
 *	ALLGLOBALS struct A under the name P
 *	We see it used with the macro AP as in AP.InOutBuf
 *	It contains objects that have dealings with the preprocessor.
 */

struct P_const {
    LIST DollarList;               /* (R) Dollar variables. Contains pointers
                                       to contents of the variables.*/
    LIST PreVarList;               /* (R) List of preprocessor variables
                                       Points to contents. Can be changed */
    LIST LoopList;                 /* (P) List of do loops */
    LIST ProcList;                 /* (P) List of procedures */
    LIST ChDollarList;             /* (P) List of Dollars changed by PP in module */
    UBYTE **PreSwitchStrings;      /* (P) The string in a switch */
    UBYTE *preStart;               /* (P) Preprocessor instruction buffer */
    UBYTE *preStop;                /* (P) end of preStart */
    UBYTE *preFill;                /* (P) Filling point in preStart */
	UBYTE *procedureExtension;     /* (P) Extension for procedure files (prc) */
	UBYTE *cprocedureExtension;    /* (P) Extension after .clear */
    int *PreIfStack;               /* (P) Tracks nesting of #if */
    int *PreSwitchModes;           /* (P) Stack of switch status */
    int *PreTypes;                 /* (P) stack of #call, #do etc nesting */
#ifdef WITHPTHREADS
	pthread_mutex_t PreVarLock;    /* (P) */
#endif
    int     PreAssignFlag;         /* (C) Indicates #assign -> catch dollar */
    int     PreContinuation;       /* (C) Indicates whether the statement is new */
    LONG    InOutBuf;              /* (P) Characters in the output buf in pre.c */
    LONG    pSize;                 /* (P) size of preStart */
    int     PreproFlag;            /* (P) Internal use to mark work on prepro instr. */
    int     iBufError;             /* (P) Flag for errors with input buffer */
    int     PreOut;                /* (P) Flag for #+ #- */
    int     PreSwitchLevel;        /* (P) Nesting of #switch */
    int     NumPreSwitchStrings;   /* (P) Size of PreSwitchStrings */
    int     MaxPreTypes;           /* (P) Size of PreTypes */
    int     NumPreTypes;           /* (P) Number of nesting objects in PreTypes */
    int     MaxPreIfLevel;         /* (C) Maximum number of nested #if. Dynamic */
    int     PreIfLevel;            /* (C) Current position if PreIfStack */
    int     DelayPrevar;           /* (P) Delaying prevar substitution */
    int     AllowDelay;            /* (P) Allow delayed prevar substitution */
    int     lhdollarerror;         /* (R) */
	int		eat;                   /* () */
	int     gNumPre;               /* (P) Number of preprocessor variables for .clear */
    int     PreDebug;              /* (C) */
    WORD    DebugFlag;             /* (P) For debugging purposes */
    WORD    preError;              /* (?) used but not defined */
    UBYTE   ComChar;               /* (P) Commentary character */
    UBYTE   cComChar;              /* (P) Old commentary character for .clear */
};

/*
 		#] P : 
 		#[ C : The C struct defines objects changed by the compiler
*/

/**
 *  The C_const struct is part of the global data and resides in the #ALLGLOBALS
 *  struct #A under the name #C.
 *  We see it used with the macro #AC as in AC.exprnames.
 *  It contains variables that involve the compiler and objects set during
 *  compilation.
 */

struct C_const {
    set_of_char separators;        /**< Separators in #call and #do */
    NAMETREE *dollarnames;         /**< [D] Names of dollar variables */
    NAMETREE *exprnames;           /**< [D] Names of expressions */
    NAMETREE *varnames;            /**< [D] Names of regular variables */
    LIST    ChannelList;           /**< Used for the #write statement. Contains #CHANNEL */
                                   /*     Later also for write? */
    LIST    DubiousList;           /**< List of dubious variables. Contains #DUBIOUSV.
                                        If not empty -> no execution */
    LIST    FunctionList;          /**< List of functions and properties. Contains #FUNCTIONS */
    LIST    ExpressionList;        /**< List of expressions, locations etc. */
    LIST    IndexList;             /**< List of indices */
    LIST    SetElementList;        /**< List of all elements of all sets */
    LIST    SetList;               /**< List of the sets */
    LIST    SymbolList;            /**< List of the symbols and their properties */
    LIST    VectorList;            /**< List of the vectors */
    LIST    PotModDolList;         /**< Potentially changed dollars */
    LIST    ModOptDolList;         /**< Module Option Dollars list */
    LIST    TableBaseList;         /**< TableBase list */
/*
    Compile buffer variables
*/
    LIST    cbufList;              /**< List of compiler buffers */
    int     cbufnum;               /**< Current compiler buffer */
/*
    Objects for auto declarations
*/
    LIST    AutoSymbolList;        /* (C) */
    LIST    AutoIndexList;         /* (C) */
    LIST    AutoVectorList;        /* (C) */
    LIST    AutoFunctionList;      /* (C) */
    NAMETREE *autonames;           /**< [D] Names in autodeclare */

    LIST    *Symbols;              /* (C) Pointer for autodeclare. Which list is
                                      it searching. Later also for subroutines */
    LIST    *Indices;              /* (C) id. */
    LIST    *Vectors;              /* (C) id. */
    LIST    *Functions;            /* (C) id. */
    NAMETREE **activenames;        /** (C) Pointer for AutoDeclare statement. Points either to
                                           varnames or autonames. */
    int     AutoDeclareFlag;       /** (C) Mode of looking for names. Set to NOAUTO (=0) or
                                           WITHAUTO (=2), cf. AutoDeclare statement */
    STREAM  *Streams;              /**< [D] The input streams. */
    STREAM  *CurrentStream;        /**< (C) The current input stream.
                                       Streams are: do loop, file, prevariable. points into Streams memory. */
    LONG    *termstack;            /**< [D] Last term statement {offset} */
    LONG    *termsortstack;        /**< [D] Last sort statement {offset} */
    WORD    *cmod;                 /**< [D] Local setting of modulus. Pointer to value. */
    WORD    *powmod;               /**< Local setting printing as powers. Points into cmod memory */
    UWORD   *modpowers;            /**< [D] The conversion table for mod-> powers. */
    WORD    *ProtoType;            /* (C) The subexpression prototype {wildcards} */
    WORD    *WildC;                /* (C) Filling point for wildcards. */
    LONG    *IfHeap;               /**< [D] Keeps track of where to go in if */
    LONG    *IfCount;              /**< [D] Keeps track of where to go in if */
    LONG    *IfStack;              /**< Keeps track of where to go in if. Points into IfHeap-memory */
    UBYTE   *iBuffer;              /**< [D] Compiler input buffer */
    UBYTE   *iPointer;             /**< Running pointer in the compiler input buffer */
    UBYTE   *iStop;                /**< Top of iBuffer */
    UBYTE   **LabelNames;          /**< [D] List of names in label statements */
    WORD    *FixIndices;           /**< [D] Buffer of fixed indices */
    WORD    *termsumcheck;         /**< [D] Checking of nesting */
    UBYTE   *WildcardNames;        /**< [D] Names of ?a variables */
    int     *Labels;               /**< Label information for during run. Pointer into LabelNames memory. */
    SBYTE   *tokens;               /**< [D] Array with tokens for tokenizer */
    SBYTE   *toptokens;            /**< Top of tokens */
    SBYTE   *endoftokens;          /**< End of the actual tokens */
    WORD    *tokenarglevel;        /**< [D] Keeps track of function arguments */
#ifdef WITHPTHREADS
    LONG    *inputnumbers;         /**< [D] For redefine */
    WORD    *pfirstnum;            /**< For redefine. Points into inputnumbers memory */
#endif
    LONG    argstack[MAXNEST];     /* (C) {contents} Stack for nesting of Argument */
    LONG    insidestack[MAXNEST];  /* (C) {contents} Stack for Argument or Inside. */
    LONG    inexprstack[MAXNEST];  /* (C) {contents} Stack for Argument or Inside. */
    LONG    iBufferSize;           /* (C) Size of the input buffer */
    LONG    TransEname;            /* (C) Used when a new definition overwrites
                                       an old expression. */
    LONG    SlavePatchSize;        /* (C) */
    LONG    mSlavePatchSize;       /* (C) */
    LONG    CModule;               /* (C) Counter of current module */
    LONG    ThreadBucketSize;      /* (C) Roughly the maximum number of input terms */
    int     NoShowInput;           /* (C) No listing of input as in .prc, #do */
    int     ShortStats;            /* (C) */
    int     compiletype;           /* (C) type of statement {DECLARATION etc} */
    int     firstconstindex;       /* (C) flag for giving first error message */
    int     insidefirst;           /* (C) Not used yet */
    int     minsidefirst;          /* (?) Not used yet */
    int     wildflag;              /* (C) Flag for marking use of wildcards */
    int     NumLabels;             /* (C) Number of labels {in Labels} */
    int     MaxLabels;             /* (C) Size of Labels array */
    int     lDefDim;               /* (C) */
    int     lDefDim4;              /* (C) */
    int     NumWildcardNames;      /* (C) Number of ?a variables */
    int     WildcardBufferSize;    /* (C) size of WildcardNames buffer */
    int     MaxIf;                 /* (C) size of IfHeap, IfSumCheck, IfCount */
    int     NumStreams;            /* (C) */
    int     MaxNumStreams;         /* (C) */
    int     firstctypemessage;     /* (C) Flag for giving first st order error */
    int     tablecheck;            /* (C) For table checking */
    int     idoption;              /* (C) */
    int     BottomLevel;           /* (C) For propercount. Not used!!! */
    int     CompileLevel;          /* (C) Subexpression level */
    int     TokensWriteFlag;       /* (C) */
    int     UnsureDollarMode;      /* (C)?Controls error messages undefined $'s */
    int     outsidefun;            /* (C) Used for writing Tables to file */
    int     funpowers;             /* (C) */
    int     WarnFlag;              /* (C) */
    int     StatsFlag;             /* (C) */
    int     NamesFlag;             /* (C) */
    int     CodesFlag;             /* (C) */
    int     SetupFlag;             /* (C) */
    int     SortType;              /* (C) */
    int     lSortType;             /* (C) */
    int     ThreadStats;           /* (C) */
    int     FinalStats;            /* (C) */
    int     ThreadsFlag;
    int     ThreadBalancing;
    int     ThreadSortFileSynch;
    int     BracketNormalize;      /* (C) Indicates whether the bracket st is normalized */
    int     maxtermlevel;          /* (C) Size of termstack */
    int     dumnumflag;            /* (C) Where there dummy indices in tokenizer? */
    int     bracketindexflag;      /* (C) Are brackets going to be indexed? */
    int     parallelflag;          /* (C) parallel allowed? */
    int     mparallelflag;         /* (C) parallel allowed in this module? */
    int     properorderflag;       /* (C) clean normalizing. */
    int     vetofilling;           /* (C) vetoes overwriting in tablebase stubs */
    int     tablefilling;          /* (C) to notify AddRHS we are filling a table */
    int     vetotablebasefill;     /* (C) For the load in tablebase */
    int     exprfillwarning;       /* (C) Warning has been printed for expressions in fill statements */
    int     lhdollarflag;          /* (R) left hand dollar present */
    int     NoCompress;            /* (R) Controls native compression */
#ifdef WITHPTHREADS
    int     numpfirstnum;          /* For redefine */
    int     sizepfirstnum;         /* For redefine */
    int     numpartodo;
#endif
    WORD    RepLevel;              /* (C) Tracks nesting of repeat. */
    WORD    arglevel;              /* (C) level of nested argument statements */
    WORD    insidelevel;           /* (C) level of nested inside statements */
    WORD    inexprlevel;           /* (C) level of nested inexpr statements */
    WORD    termlevel;             /* (C) level of nested term statements */
    WORD    argsumcheck[MAXNEST];  /* (C) Checking of nesting */
    WORD    insidesumcheck[MAXNEST];/* (C) Checking of nesting */
    WORD    inexprsumcheck[MAXNEST];/* (C) Checking of nesting */
    WORD    MustTestTable;         /* (C) Indicates whether tables have been changed */
    WORD    DumNum;                /* (C) */
    WORD    ncmod;                 /* (C) Local setting of modulus. size of cmod */
    WORD    npowmod;               /* (C) Local printing as powers. size powmod */
    WORD    DirtPow;               /* (C) Flag for changes in printing mod powers */
    WORD    lUnitTrace;            /* (C) Local value of Tr[1] */
    WORD    NwildC;                /* (C) Wildcard counter */
    WORD    ComDefer;              /* (C) defer brackets */
    WORD    CollectFun;            /* (C) Collect function number */
    WORD    AltCollectFun;         /* (C) Alternate Collect function number */
    WORD    OutputMode;            /* (C) */
    WORD    OutputSpaces;          /* (C) */
    WORD    OutNumberType;         /* (C) Controls RATIONAL/FLOAT output */
    WORD    lUniTrace[4];          /* (C) */
    WORD    RepSumCheck[MAXREPEAT];/* (C) Checks nesting of repeat, if, argument */
    WORD    DidClean;              /* (C) Test whether nametree needs cleaning */
    WORD    IfLevel;               /* (C) */
    WORD    WhileLevel;            /* (C) */
    WORD    *IfSumCheck;           /**< [D] Keeps track of where to go in if */
    WORD    LogHandle;             /* (C) The Log File */
    WORD    LineLength;            /* (C) */
    WORD    StoreHandle;           /* (C) Handle of .str file */
    WORD    HideLevel;             /* (C) Hiding indicator */
    WORD    lPolyFun;              /* (C) local value of PolyFun */
    WORD    lPolyFunType;          /* (C) local value of PolyFunType */
    WORD    SymChangeFlag;         /* (C) */
    WORD    CollectPercentage;     /* (C) Collect function percentage */
    WORD    ShortStatsMax;         /* For  On FewerStatistics 10; */
#ifdef PARALLEL
    WORD NumberOfRhsExprInModule;  /* (C) Number of RHS expressions*/
    WORD NumberOfRedefsInModule;   /* (C) Number of redefined variables in the module*/
#endif
    UBYTE   Commercial[COMMERCIALSIZE+2]; /* (C) Message to be printed in statistics */
    int     CheckpointFlag;        /**< Tells preprocessor whether checkpoint code must executed.
                                        -1 : do recovery from snapshot, set by command line option;
                                        0 : do nothing; 1 : create snapshots, set by On checkpoint
                                        statement */
    LONG    CheckpointStamp;       /**< Timestamp of the last created snapshot (set to Timer(0)).*/
    char    *CheckpointRunAfter;   /**< [D] Filename of script to be executed _before_ creating the
                                        snapshot. =0 if no script shall be executed. */
    char    *CheckpointRunBefore;  /**< [D] Filename of script to be executed _after_ having
                                        created the snapshot. =0 if no script shall be executed.*/
    LONG    CheckpointInterval;    /**< Time interval in milliseconds for snapshots. =0 if
                                        snapshots shall be created at the end of _every_ module.*/
};
/*
 		#] C : 
 		#[ S : The S struct defines objects changed at the start of the run (Processor)
		       Basically only set by the master.
*/
/**
 *	The S_const struct is part of the global data and resides in the
 *	ALLGLOBALS struct A under the name S
 *	We see it used with the macro AS as in AS.ExecMode
 *	It has some variables used by the master in multithreaded runs
 */

struct S_const {
#ifdef WITHPTHREADS
	pthread_mutex_t	inputslock;
	pthread_mutex_t	outputslock;
#endif
    POSITION *OldOnFile;           /* (S) File positions of expressions */
    int     NumOldOnFile;          /* (S) Number of expressions in OldOnFile */
    int     MultiThreaded;         /* (S) Are we running multi-threaded? */
#ifdef WITHPTHREADS
    int     MasterSort;            /* Final stage of sorting to the master */
#endif
    int     Balancing;             /* For second stage loadbalancing */
    WORD    ExecMode;              /* (S) */

    WORD    CollectOverFlag;       /* (R) Indicates overflow at Collect */
#ifdef WITHPTHREADS
	WORD	sLevel;                /* Copy of AR0.sLevel because it can get messy */
#endif
};
/*
 		#] S : 
 		#[ R : The R struct defines objects changed at run time.
               They determine the environment that has to be transfered
               together with a term during multithreaded execution.
*/
/**
 *  The R_const struct is part of the global data and resides either in the
 *  ALLGLOBALS struct A, or the ALLPRIVATES struct B (TFORM) under the name R
 *  We see it used with the macro AR as in AR.infile
 *  It has the variables that define the running environment and that
 *  should be transferred with a term in a multithreaded run.
 */

struct R_const {
    FILEHANDLE *infile;            /* (R) Points alternatingly to Fscr[0] or Fscr[1] */
    FILEHANDLE *outfile;           /* (R) Points alternatingly to Fscr[1] or Fscr[0] */
    FILEHANDLE *hidefile;          /* (R) Points to Fscr[2] */
    FILEDATA    StoreData;         /* (O) */

    WORD    *CompressBuffer;       /* (M) */
    WORD    *ComprTop;             /* (M) */
    WORD    *CompressPointer;      /* (R) */
    VOID    *CompareRoutine;
    FILEHANDLE  Fscr[3];           /* (R) Dollars etc play with it too */
    FILEHANDLE  FoStage4[2];       /* (R) In Sort. Stage 4. */

    POSITION DefPosition;          /* (R) Deferred position of keep brackets. */
#ifdef WITHPOSIXCLOCK
    struct timespec timing;        /* Struct defined in time.h */
#endif
    LONG    OldTime;               /* (R) Zero time. Needed in timer. */
    LONG    InInBuf;               /* (R) Characters in input buffer. Scratch files. */
    LONG    pWorkSize;             /* (R) Size of pWorkSpace */
    LONG    lWorkSize;             /* (R) Size of lWorkSpace */
    LONG    posWorkSize;           /* (R) Size of posWorkSpace */
    int     NoCompress;            /* (R) Controls native compression */
    int     gzipCompress;          /* (R) Controls gzip compression */
    int     Cnumlhs;               /* Local copy of cbuf[rbufnum].numlhs */
#ifdef WITHPTHREADS
    int     exprtodo;              /* The expression to do in parallel mode */
#endif
    WORD    GetFile;               /* (R) Where to get the terms {like Hide} */
    WORD    KeptInHold;            /* (R) */
    WORD    BracketOn;             /* (R) Intensly used in poly_ */
    WORD    MaxBracket;            /* (R) Size of BrackBuf. Changed by poly_ */
    WORD    CurDum;                /* (R) Current maximum dummy number */
    WORD    DeferFlag;             /* (R) For defered brackets */
    WORD    TePos;                 /* (R) */
    WORD    sLevel;                /* (R) Sorting level */
    WORD    Stage4Name;            /* (R) Sorting only */
    WORD    GetOneFile;            /* (R) Getting from hide or regular */
    WORD    PolyFun;               /* (C) Number of the PolyFun function */
    WORD    PolyFunType;           /* () value of PolyFunType */
    WORD    Eside;                 /* () Tells which side of = sign */
    WORD    MaxDum;                /* Maximum dummy value in an expression */
    WORD    level;                 /* Running level in Generator */
    WORD    expchanged;            /* (R) Info about expression */
    WORD    expflags;              /* (R) Info about expression */
    WORD    CurExpr;               /* (S) Number of current expression */
    WORD    SortType;              /* A copy of AC.SortType to play with */
    WORD    ShortSortCount;        /* For On FewerStatistics 10; */
};

/*
 		#] R : 
 		#[ T : These are variables that stay in each thread during multi threaded execution.
*/
/**
 *	The T_const struct is part of the global data and resides either in the
 *	ALLGLOBALS struct A, or the ALLPRIVATES struct B (TFORM) under the name T
 *	We see it used with the macro AT as in AT.WorkPointer
 *	It has variables that are private to each thread, most of which have
 *	to be defined at startup.
 */

struct T_const {
#ifdef WITHPTHREADS
    SORTBLOCK SB;
#endif
    SORTING *S0;                   /* (-) The thread specific sort buffer */
    SORTING *SS;                   /* (R) Current sort buffer */
    NESTING     Nest;              /* (R) Nesting of function levels etc. */
    NESTING     NestStop;          /* (R) */
    NESTING     NestPoin;          /* (R) */
    WORD    *BrackBuf;             /* (R) Bracket buffer. Used by poly_ at runtime. */
    STORECACHE  StoreCache;        /* (R) Cache for picking up stored expr. */
                                   /*     Seems not to be active */
    WORD    **pWorkSpace;          /* (R) Workspace for pointers. Dynamic. */
    LONG    *lWorkSpace;           /* (R) WorkSpace for LONG. Dynamic. */
    POSITION *posWorkSpace;        /* (R) WorkSpace for file positions */
    WORD    *WorkSpace;            /* (M) */
    WORD    *WorkTop;              /* (M) */
    WORD    *WorkPointer;          /* (R) Pointer in the WorkSpace heap. */
    int     *RepCount;             /* (M) Buffer for repeat nesting */
    int     *RepTop;               /* (M) Top of RepCount buffer */
    WORD    *WildArgTaken;         /* (N) Stack for wildcard pattern matching */
    WORD    *n_coef;               /* (M) Used by normal. local. */
    WORD    *n_llnum;              /* (M) Used by normal. local. */
    UWORD   *factorials;           /* (T) buffer of factorials. Dynamic. */
    UWORD   *bernoullis;           /* (T) The buffer with bernoulli numbers. Dynamic. */
	WORD    *primelist;
    WORD    *lastpolyrem;          /* () Remainder after PolyDiv_ */
    long    *pfac;                 /* (T) array of positions of factorials. Dynamic. */
    long    *pBer;                 /* (T) array of positions of Bernoulli's. Dynamic. */
    ULONG   *wranfia;
    WORD    *TMaddr;               /* (R) buffer for TestSub */
    WORD    *WildMask;             /* (N) Wildcard info during pattern matching */
    WORD    *previousEfactor;      /* () Cache for factors in expressions */
    WORD    **TermMemHeap;        /* For TermMalloc. Set zero in Checkpoint */
    UWORD    **NumberMemHeap;      /* For NumberMalloc. Set zero in Checkpoint */
    LONG    sBer;                  /* (T) Size of the bernoullis buffer */
    LONG    pWorkPointer;          /* (R) Offset-pointer in pWorkSpace */
    LONG    lWorkPointer;          /* (R) Offset-pointer in lWorkSpace */
    LONG    posWorkPointer;        /* (R) Offset-pointer in posWorkSpace */
    int     sfact;                 /* (T) size of the factorials buffer */
    int     mfac;                  /* (T) size of the pfac array. */
    int     ebufnum;               /* (R) extra compiler buffer */
    int     WildcardBufferSize;    /* () local copy for updates */
#ifdef WITHPTHREADS
    int     identity;              /* () When we work with B->T */
    int     LoadBalancing;         /* Needed for synchronization */
#ifdef WITHSORTBOTS
    int     SortBotIn1;            /* Input stream 1 for a SortBot */
    int     SortBotIn2;            /* Input stream 2 for a SortBot */
#endif
#endif
    int     wranfcall;
    int     TermMemMax;            /* For TermMalloc. Set zero in Checkpoint */
    int     TermMemTop;            /* For TermMalloc. Set zero in Checkpoint */
    int     NumberMemMax;          /* For NumberMalloc. Set zero in Checkpoint */
    int     NumberMemTop;          /* For NumberMalloc. Set zero in Checkpoint */
    WORD    dummysubexp[SUBEXPSIZE+4]; /* () used in normal.c */
    WORD    onesympol[9];          /* () Used in poly.c = {8,SYMBOL,4,1,1,1,1,3,0} */
    WORD    comsym[8];             /* () Used in tools.c = {8,SYMBOL,4,0,1,1,1,3} */
    WORD    comnum[4];             /* () Used in tools.c = { 4,1,1,3 } */
    WORD    comfun[FUNHEAD+4];     /* () Used in tools.c = {7,FUNCTION,3,0,1,1,3} */
                                   /*            or { 8,FUNCTION,4,0,0,1,1,3 } */
    WORD    comind[7];             /* () Used in tools.c = {7,INDEX,3,0,1,1,3} */
    WORD    MinVecArg[7+ARGHEAD];  /* (N) but should be more local */
    WORD    FunArg[4+ARGHEAD+FUNHEAD]; /* (N) but can be more local */
    WORD    zeropol[1];            /* () Polynomial with value zero */
    WORD    onepol[5];             /* () Polynomial with value one */
    WORD    locwildvalue[SUBEXPSIZE]; /* () Used in argument.c = {SUBEXPRESSION,SUBEXPSIZE,0,0,0} */
    WORD    mulpat[SUBEXPSIZE+5];  /* () Used in argument.c = {TYPEMULT, SUBEXPSIZE+3, 0, */
                                   /* SUBEXPRESSION, SUBEXPSIZE, 0, 1, 0, 0, 0 } */
    WORD    proexp[SUBEXPSIZE+5];  /* () Used in poly.c */
    WORD    TMout[40];             /* (R) Passing info */
    WORD    TMbuff;                /* (R) Communication between TestSub and Genera */
    WORD    nfac;                  /* (T) Number of highest stored factorial */
    WORD    nBer;                  /* (T) Number of highest bernoulli number. */
    WORD    mBer;                  /* (T) Size of buffer pBer. */
    WORD    PolyAct;               /* (R) Used for putting the PolyFun at end. ini at 0 */
    WORD    RecFlag;               /* (R) Used in TestSub. ini at zero. */
	WORD    inprimelist;
	WORD    sizeprimelist;
};
/*
 		#] T :
 		#[ N : The N struct contains variables used in running information
               that is inside blocks that should not be split, like pattern
               matching, traces etc. They are local for each thread.
               They don't need initializations.
*/
/**
 *	The N_const struct is part of the global data and resides either in the
 *	ALLGLOBALS struct A, or the ALLPRIVATES struct B (TFORM) under the name N
 *	We see it used with the macro AN as in AN.RepFunNum
 *	It has variables that are private to each thread and are used as
 *	temporary storage during the expansion of the terms tree.
 */

struct N_const {
    LONG    *polybpstack;          /* () Used in poly */
    WORD    *EndNest;              /* (R) Nesting of function levels etc. */
    WORD    *Frozen;               /* (R) Bracket info */
    WORD    *FullProto;            /* (R) Prototype of a subexpression or table */
    WORD    *cTerm;                /* (R) Current term for coef_ and term_ */
    int     *RepPoint;             /* (R) Pointer in RepCount buffer. Tracks repeat */
    WORD    *WildValue;            /* (N) Wildcard info during pattern matching */
    WORD    *WildStop;             /* (N) Wildcard info during pattern matching */
    WORD    *argaddress;           /* (N) Used in pattern matching of arguments */
    WORD    *RepFunList;           /* (N) For pattern matching */
    WORD    *patstop;              /* (N) Used in pattern matching */
    WORD    *terstop;              /* (N) Used in pattern matching */
    WORD    *terstart;             /* (N) Used in pattern matching */
    WORD    *DumFound;             /* (N) For renumbering indices {make local?} */
    WORD    **DumPlace;            /* (N) For renumbering indices {make local?} */
    WORD    **DumFunPlace;         /* (N) For renumbering indices {make local?} */
    WORD    *UsedSymbol;           /* (N) When storing terms of a global expr. */
    WORD    *UsedVector;           /* (N) When storing terms of a global expr. */
    WORD    *UsedIndex;            /* (N) When storing terms of a global expr. */
    WORD    *UsedFunction;         /* (N) When storing terms of a global expr. */
    WORD    *MaskPointer;          /* (N) For wildcard pattern matching */
    WORD    *ForFindOnly;          /* (N) For wildcard pattern matching */
    WORD    *findTerm;             /* (N) For wildcard pattern matching */
    WORD    *findPattern;          /* (N) For wildcard pattern matching */
    WORD    *brackbuf;             /* () Used in poly */
    WORD    *polybrackets;         /* () Used in poly */
#ifdef WITHZLIB
	Bytef	**ziobufnum;           /* () Used in compress.c */
	Bytef	*ziobuffers;           /* () Used in compress.c */
#endif
	WORD	*dummyrenumlist;       /* () Used in execute.c and store.c */
	WORD	*mgscr1;               /* () Used in factor.c */
	WORD	*mgscr2;               /* () Used in factor.c */
	WORD	*mgscr3;               /* () Used in factor.c */
	int		*funargs;              /* () Used in lus.c */
	WORD	**funlocs;             /* () Used in lus.c */
	int		*funinds;              /* () Used in lus.c */
	UWORD	*NoScrat2;             /* () Used in normal.c */
	WORD	*ReplaceScrat;         /* () Used in normal.c */
/*
	WORD	*st;
	WORD	*sm;
	WORD	*sr;
	WORD	*sc;
*/
	TRACES	*tracestack;           /* () used in opera.c */
	WORD	*selecttermundo;       /* () Used in pattern.c */
	WORD	*patternbuffer;        /* () Used in pattern.c */
	WORD	*PoinScratch[300];     /* () used in reshuf.c */
	WORD	*FunScratch[300];      /* () used in reshuf.c */
	FUN_INFO *FunInfo;             /* () Used in smart.c */
	WORD	**SplitScratch;        /* () Used in sort.c */
	SORTING **FunSorts;            /* () Used in sort.c */
	UWORD	*SoScratC;             /* () Used in sort.c */
	WORD	*listinprint;          /* () Used in proces.c and message.c */
	WORD	*currentTerm;          /* () Used in proces.c and message.c */
	WORD	**arglist;             /* () Used in function.c */
	int		*tlistbuf;             /* () used in lus.c */
#ifdef WHICHSUBEXPRESSION
	UWORD	*BinoScrat;            /* () Used in proces.c */
#endif
	WORD	*compressSpace;        /* () Used in sort.c */
	WORD	*poly1a;
	WORD	*poly2a;
#ifdef WITHPTHREADS
	THREADBUCKET *threadbuck;
#endif
	POLYMOD polymod1;              /* For use in PolyModGCD and calling routines */
	POLYMOD polymod2;              /* For use in PolyModGCD and calling routines */
    POSITION OldPosIn;             /* (R) Used in sort. */
    POSITION OldPosOut;            /* (R) Used in sort */
	POSITION theposition;          /* () Used in index.c */
    LONG    polybpointer;          /* () Used in poly */
    LONG    polybsize;             /* () Used in poly */
    LONG    polybpsize;            /* () Used in poly */
	LONG	deferskipped;          /* () Used in proces.c store.c and parallel.c */
	LONG	InScratch;             /* () Used in sort.c */
	LONG	SplitScratchSize;      /* () Used in sort.c */
	LONG	ninterms;              /* () Used in proces.c and sort.c */
#ifdef WITHPTHREADS
	LONG	inputnumber;           /* () For use in redefine */
#endif
#ifdef WHICHSUBEXPRESSION
	LONG	last2;                 /* () Used in proces.c */
	LONG	last3;                 /* () Used in proces.c */
#endif
    int     NumTotWildArgs;        /* (N) Used in pattern matching */
    int     UseFindOnly;           /* (N) Controls pattern routines */
    int     UsedOtherFind;         /* (N) Controls pattern routines */
    int     doingpoly;             /* () Used in poly */
    int     sorttype;              /* () Used in poly */
    int     ErrorInDollar;         /* (R) */
	int		numfargs;              /* () Used in lus.c */
	int		numflocs;              /* () Used in lus.c */
	int		nargs;                 /* () Used in lus.c */
	int		tohunt;                /* () Used in lus.c */
	int		numoffuns;             /* () Used in lus.c */
	int		funisize;              /* () Used in lus.c */
	int		RSsize;                /* () Used in normal.c */
	int		numtracesctack;        /* () used in opera.c */
	int		intracestack;          /* () used in opera.c */
	int		numfuninfo;            /* () Used in smart.c */
	int		NumFunSorts;           /* () Used in sort.c */
	int		MaxFunSorts;           /* () Used in sort.c */
	int		arglistsize;           /* () Used in function.c */
	int		tlistsize;             /* () used in lus.c */
	int		filenum;               /* () used in setfile.c */
	int		compressSize;          /* () Used in sort.c */
	int		polysortflag;
	WORD	RenumScratch[300];     /* () used in reshuf.c */
    WORD    oldtype;               /* (N) WildCard info at pattern matching */
    WORD    oldvalue;              /* (N) WildCard info at pattern matching */
    WORD    NumWild;               /* (N) Used in Wildcard */
    WORD    RepFunNum;             /* (N) Used in pattern matching */
    WORD    DisOrderFlag;          /* (N) Disorder option? Used in pattern matching */
    WORD    WildDirt;              /* (N) dirty in wldcard substitution. */
    WORD    NumFound;              /* (N) in reshuf only. Local? */
    WORD    WildReserve;           /* (N) Used in the wildcards */
    WORD    TeInFun;               /* (R) Passing type of action */
    WORD    TeSuOut;               /* (R) Passing info. Local? */
    WORD    WildArgs;              /* (R) */
    WORD    WildEat;               /* (R) */
    WORD    PolyNormFlag;          /* (R) For polynomial arithmetic */
    WORD    bracketon;             /* () Used in poly */
    WORD    maxbracket;            /* () Used in poly */
    WORD    polyblevel;            /* () Used in poly */
	WORD	nmgscr;                /* () Used in factor.c */
	WORD	sizeselecttermundo;    /* () Used in pattern.c */
	WORD	patternbuffersize;     /* () Used in pattern.c */
	WORD	numlistinprint;        /* () Used in process.c */
	WORD	getdivgcd;
#ifdef WHICHSUBEXPRESSION
	WORD	nbino;                 /* () Used in proces.c */
	WORD	last1;                 /* () Used in proces.c */
#endif
};

/*
 		#] N : 
 		#[ O : The O struct concerns output variables
*/
/**
 *	The O_const struct is part of the global data and resides in the
 *	ALLGLOBALS struct A under the name O
 *	We see it used with the macro AO as in AO.OutputLine
 *	It contains variables that involve the writing of text output and
 *	save/store files.
 */

struct O_const {
    UBYTE   *OutputLine;           /* (O) Sits also in debug statements */
    UBYTE   *OutStop;              /* (O) Top of OutputLine buffer */
    UBYTE   *OutFill;              /* (O) Filling point in OutputLine buffer */
    WORD    *bracket;              /* (O) For writing brackets */
    WORD    *termbuf;              /* (O) For writing terms */
    UBYTE   *wpos;                 /* (O) Only when storing file {local?} */
    UBYTE   *wpoin;                /* (O) Only when storing file {local?} */
    UBYTE   *DollarOutBuffer;      /* (O) Outputbuffer for Dollars */
    UBYTE   *CurBufWrt;            /* (O) Name of currently written expr. */
    FILEDATA    SaveData;          /* (O) */
    STOREHEADER SaveHeader;        /* ()  System Independent save-Files */
    WORD    transFlag;             /* ()  >0 indicades that translations have to be done */
    WORD    powerFlag;             /* ()  >0 indicades that some exponents/powers had to be adjusted */
    WORD    resizeFlag;            /* ()  >0 indicades that something went wrong when resizing words */
    WORD    bufferedInd;           /* ()  Contains extra INDEXENTRIES, see ReadSaveIndex() for an explanation */
    VOID    (*FlipWORD)(UBYTE *);  /* ()  Function pointers for translations. Initialized by ReadSaveHeader() */
    VOID    (*FlipLONG)(UBYTE *);
    VOID    (*FlipPOS)(UBYTE *);
    VOID    (*FlipPOINTER)(UBYTE *);
    VOID    (*ResizeData)(UBYTE *,int,UBYTE *,int);
    VOID    (*ResizeWORD)(UBYTE *,UBYTE *);
    VOID    (*ResizeNCWORD)(UBYTE *,UBYTE *);
    VOID    (*ResizeLONG)(UBYTE *,UBYTE *);
    VOID    (*ResizePOS)(UBYTE *,UBYTE *);
    VOID    (*ResizePOINTER)(UBYTE *,UBYTE *);
    VOID    (*CheckPower)(UBYTE *);
    VOID    (*RenumberVec)(UBYTE *);
	UBYTE	*tensorList;           /* Dynamically allocated list with functions that are tensorial. */
#ifdef mBSD
#ifdef MICROTIME
    long    wrap;                  /* (O) For statistics time. wrap around */
    long    wrapnum;               /* (O) For statistics time. wrap around */
#endif
#endif
/*
#ifdef _MSC_VER
    long    wrap;
    long    wrapnum;
#endif
*/
    LONG    NumInBrack;            /* (O) For typing [] option in print */
    LONG    wlen;                  /* (O) Used to store files. */
    LONG    DollarOutSizeBuffer;   /* (O) Size of DollarOutBuffer */
    LONG    DollarInOutBuffer;     /* (O) Characters in DollarOutBuffer */
    int     OutInBuffer;           /* (O) Which routine does the writing */
    int     NoSpacesInNumbers;     /*     For very long numbers */
    int     BlockSpaces;           /*     For very long numbers */
    WORD    OutSkip;               /* (O) How many chars to skip in output line */
    WORD    IsBracket;             /* (O) Controls brackets */
    WORD    InFbrack;              /* (O) For writing only */
    WORD    PrintType;             /* (O) */
    WORD    FortFirst;             /* (O) Only in sch.c */
    WORD    DoubleFlag;            /* (O) Output in double precision */
    WORD    IndentSpace;           /* For indentation in output */
    UBYTE   FortDotChar;           /* (O) */
};
/*
 		#] O : 
 		#[ X : The X struct contains variables that deal with the external channel
*/
/**
 *	The X_const struct is part of the global data and resides in the
 *	ALLGLOBALS struct A under the name X
 *	We see it used with the macro AX as in AX.timeout
 *	It contains variables that involve communication with external programs
 */

struct X_const {
	UBYTE *currentPrompt;
	int timeout;               /* timeout to initialize preset channels.
	                              If timeout<0, the preset channels are 
	                              already initialized*/
	int killSignal;            /* signal number, SIGKILL by default*/
	int killWholeGroup;        /* if 0, the signal is sent only to a process, 
	                              if !=0 (default) is sent to a whole process group*/
	int daemonize;             /* if !=0 (default), start in a daemon mode */
	UBYTE *shellname;          /* if !=NULL (default is "/bin/sh -c"), start in 
	                              the specified subshell*/
	UBYTE *stderrname;         /* If !=NULL (default if "/dev/null"), stderr is 
	                              redirected to the specified file*/
	int	currentExternalChannel;
};
/*
 		#] X : 
 		#[ Definitions :
*/

#ifdef WITHPTHREADS

/**
 *	With pthreads (TFORM) the ALLGLOBALS struct has all the variables of which
 *	there is only a single copy.
 */

typedef struct AllGlobals {
    struct M_const M;
    struct P_const P;
    struct C_const C;
    struct S_const S;
    struct O_const O;
	struct X_const X;
} ALLGLOBALS;

/**
 *	With pthreads (TFORM) the ALLPRIVATES struct has all the variables of which
 *	each thread must have its own (private) copy.
 */

typedef struct AllPrivates {
    struct R_const R;
    struct T_const T;
    struct N_const N;
} ALLPRIVATES;

#else

/**
 *	Without pthreads (FORM) the ALLGLOBALS struct has all the global variables
 */

typedef struct AllGlobals {
    struct M_const M;
    struct P_const P;
    struct C_const C;
    struct S_const S;
    struct R_const R;
    struct T_const T;
    struct N_const N;
    struct O_const O;
	struct X_const X;
} ALLGLOBALS;

#endif

/*
 		#] Definitions : 
    #] A :
  	#[ FG :
*/

#ifdef ANSI
typedef WORD (*WCN)(PHEAD WORD *,WORD *,WORD,WORD);
typedef WORD (*WCN2)(PHEAD WORD *,WORD *);
#else
typedef WORD (*WCN)();
typedef WORD (*WCN2)();
#endif

/**
 *	The FIXEDGLOBALS struct is an anachronism. It started as the struct
 *	with global variables that needed initialization.
 *	It contains the elements Operation and OperaFind which define a very early
 *	way of automatically jumping to the proper operation. We find the results
 *	of it in parts of the file opera.c
 *	Later operations were treated differently in a more transparent way.
 *	We never changed the existing code. The most important part is currently
 *	the cTable which is used intensively in the compiler.
 */

typedef struct FixedGlobals {
	WCN		Operation[8];
	WCN2	OperaFind[6];
	char	*VarType[10];
	char	*ExprStat[17];
	char	*FunNam[2];
	char	*swmes[3];
	char	*fname;
	UBYTE	*s_one;
	UINT	cTable[256];
} FIXEDGLOBALS;

/*
  	#] FG : 
*/

#endif
