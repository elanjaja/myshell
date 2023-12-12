#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

/* for read/write buffers */
#define READ_BUF_SIZE 1024
#define WRITE_BUF_SIZE 1024
#define BUF_FLUSH -1

/* for command chaining */
#define CMD_NORM	0
#define CMD_OR		1
#define CMD_AND		2
#define CMD_CHAIN	3

/* for convert_number() */
#define CONVERT_LOWERCASE	1
#define CONVERT_UNSIGNED	2

/* 1 if using system getline() */
#define USE_GETLINE 0
#define USE_STRTOK 0

#define HIST_FILE	".simple_shell_history"
#define HIST_MAX	4096

extern char **environ;


/**
 * struct liststr - singly linked list
 * @num: the number field
 * @str: a string
 * @next: points to the next node
 */
typedef struct liststr
{
	int num;
	char *str;
	struct liststr *next;
} list_t;

/**
 *struct passdata - contains pseudo-arguements to pass into a function,
 *		allowing uniform prototype for function pointer struct
 *@arg: a string generated from getline containing arguements
 *@argv: an array of strings generated from arg
 *@path: a string path for the current command
 *@argc: the argument count
 *@line_count: the error count
 *@err_num: the error code for exit()s
 *@linecount_flag: if on count this line of input
 *@fname: the program filename
 *@env: linked list local copy of environ
 *@environ: custom modified copy of environ from LL env
 *@history: the history node
 *@alias: the alias node
 *@env_changed: on if environ was changed
 *@status: the return status of the last exec'd command
 *@cmd_buf: address of pointer to cmd_buf, on if chaining
 *@cmd_buf_type: CMD_type ||, &&, ;
 *@readfd: the fd from which to read line input
 *@histcount: the history line number count
 */
typedef struct passdata
{
	char *arg;
	char **argv;
	char *path;
	int argc;
	unsigned int linecount;
	int err_num;
	int linecount_flag;
	char *fname;
	list_t *env;
	list_t *history;
	list_t *alias;
	char **environ;
	int env_changed;
	int status;

	char **cmd_buf; /* pointer to cmd ; chain buffer, for memory mangement */
	int cmd_buf_type; /* CMD_type ||, &&, ; */
	int readfd;
	int histcount;
} data_t;

#define INFO_INIT \
{NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, \
	0, 0, 0}

/**
 *struct builtin - contains a builtin string and related function
 *@type: the builtin command flag
 *@func: the function
 */
typedef struct builtin
{
	char *type;
	int (*func)(data_t *);
} builtin_table;


/* toem_shloop.c */
int shell_loop(data_t *, char **);
int find_builtin_command(data_t *);
void find_cmd(data_t *);
void execute_command(data_t *);

/* toem_parser.c */
int executable(data_t *, char *);
char *copy_chars(char *, int, int);
char *find_path(data_t *, char *, char *);

/* loophsh.c */
int loophsh(char **);

/* toem_errors.c */
void error_puts(char *);
int error_putchar(char);
int _putfd(char c, int fd);
int _putsfd(char *str, int fd);

/* toem_string.c */
int string_length(char *);
int string_compare(char *, char *);
char *starts_with(const char *, const char *);
char *_catstr(char *, char *);

/* toem_string1.c */
char *copy_string(char *, char *);
char *_strdup(const char *);
void print_string(char *);
int write_character(char);

/* toem_exits.c */
char *_cpystr(char *, char *, int);
char *_strncat(char *, char *, int);
char *str_chr(char *, char);

/* toem_tokenizer.c */
char **split_string(char *, char *);
char **split_string_alt(char *, char);

/* toem_realloc.c */
char *_memset(char *, char, unsigned int);
void free_string(char **);
void *_realloc(void *, unsigned int, unsigned int);

/* toem_memory.c */
int bfree(void **);

/* toem_atoi.c */
int interactive_mood(data_t *);
int delimeter(char, char *);
int alphabetic(int);
int _atoi(char *);

/* toem_errors1.c */
int _erratoi(char *);
void print_error(data_t *, char *);
int print_decimal(int, int);
char *convert_string(long int, int, int);
void replace_comments(char *);

/* toem_builtin.c */
int shell_exit(data_t *);
int my_cd(data_t *);
int my_help(data_t *);

/* toem_builtin1.c */
int display_history(data_t *);
int manage_alias(data_t *);

/*toem_getline.c */
ssize_t get_input(data_t *);
int _getline(data_t *, char **, size_t *);
void interruptHandler(int);

/* toem_getinfo.c */
void clear_data(data_t *);
void set_data(data_t *, char **);
void free_data(data_t *, int);

/* toem_environ.c */
char *_getenv(data_t *);
int print_env(data_t *);
int my_setenv(data_t *);
int _myunsetenv(data_t *);
int populate_env(data_t *);

/* toem_getenv.c */
char **environ_string(data_t *);
int unset_env(data_t *, char *);
int set_env(data_t *, char *, char *);

/* toem_history.c */
char *get_history(data_t *data);
int write_history(data_t *data);
int read_history(data_t *data);
int build_history(data_t *data, char *buf, int linecount);
int renumber_history(data_t *data);

/* toem_lists.c */
list_t *add_node(list_t **, const char *, int);
list_t *add_node_at_end(list_t **, const char *, int);
size_t print_list_string(const list_t *);
int delete_node_index(list_t **, unsigned int);
void free_list_node(list_t **);

/* toem_lists1.c */
size_t length_list(const list_t *);
char **list_to_strings(list_t *);
size_t print_list(const list_t *);
list_t *node_starts(list_t *, char *, char);
ssize_t node_index(list_t *, list_t *);

/* toem_vars.c */
int chain_delimeter(data_t *, char *, size_t *);
void checkchain(data_t *, char *, size_t *, size_t, size_t);
int restore_alias(data_t *);
int restore_vars(data_t *);
int replace_string(char **, char *);

#endif
