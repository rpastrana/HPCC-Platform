#ifndef _DOCUMENTATION_MARKUP_HPP
#define _DOCUMENTATION_MARKUP_HPP

static const char* DM_SECT2 = "sect2";
static const char* DM_SECT2_END = "</sect2>\n";
static const char* DM_SECT3_BEGIN = "<sect3>\n";
static const char* DM_SECT3_END = "</sect3>\n";
static const char* DM_SECT4_BEGIN = "<sect4>\n";
static const char* DM_SECT4_END = "</sect4>\n";
static const char* DM_ID = "id";
//static const char* DM_TITLE = "title";
static const char* DM_TITLE = "title";
static const char* DM_TITLE_BEGIN = "<title>";
static const char* DM_TITLE_END = "</title>\n";
static const char* DM_TITLE_LITERAL = "title";

static const char* DM_TABLE_BEGIN = "<informaltable colsep=\"1\" rowsep=\"1\">\n";
static const char* DM_TABLE_END = "</informaltable>\n";
static const char* DM_TABLE_ROW = "row";
static const char* DM_TABLE_ENTRY = "entry";
static const char* DM_PARA = "para";
static const char* DM_TGROUP = "tgroup";
static const char* DM_TGROUP4_BEGIN = "<tgroup cols=\"4\" align=\"left\">\n";
static const char* DM_TGROUP4_END = "</tgroup>\n";
static const char* DM_TGROUP_END = "</tgroup>\n";
//static const char* DM_TBODY_BEGIN = "<tbody>\n";
static const char* DM_TBODY_BEGIN = "<thead><row>\n<entry>attribute</entry>\n<entry>values</entry>\n<entry>default</entry>\n<entry>required</entry>\n</row>\n</thead><tbody>\n";
static const char* DM_TBODY_END = "</tbody>\n";
static const char* DM_COL_SPEC4 = "<colspec colwidth=\"125pt\" /><colspec colwidth=\"195pt\" />\n";


#endif // _DOCUMENTATION_MARKUP_HPP
