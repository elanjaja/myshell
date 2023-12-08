#include "main.h"

/**
 * input_buffer - buffers chained commands
 * @data: parameter struct
 * @buf: address of buffer
 * @len: address of len var
 *
 * Return: bytes read
 */
ssize_t input_buffer(data_t *data, char **buf, size_t *len)
{
	ssize_t br = 0;
	size_t len_p = 0;

	if (!*len) /* if nothing left in the buffer, fill it */
	{
		free(*buf);
		*buf = NULL;
		signal(SIGINT, interruptHandler);
#if USE_GETLINE
		br = getline(buf, &len_p, stdin);
#else
		br = getline(data, buf, &len_p);
#endif
		if (br > 0)
		{
			if ((*buf)[br - 1] == '\n')
			{
				(*buf)[br - 1] = '\0'; /* remove trailing newline */
				br--;
			}
			data->linecount_flag = 1;
			replace_comments(*buf);
			build_history(data, *buf, data->histcount++);
			/* if (string_chr(*buf, ';')) is this a command chain? */
			{
				*len = br;
				data->cmd_buf = buf;
			}
		}
	}
	return (br);
}

/**
 * get_input - gets a line minus the newline
 * @data: parameter struct
 *
 * Return: bytes read
 */
ssize_t get_input(data_t *data)
{
	static char *buf; /* the ';' command chain buffer */
	static size_t a, j, len;
	ssize_t br = 0;
	char **buf_p = &(data->arg), *p;

	write_character(BUF_FLUSH);
	br = input_buffer(data, &buf, &len);
	if (br == -1) /* EOF */
		return (-1);
	if (len)	/* commands left in the chain buffer */
	{
		j = a;
		p = buf + a;

		checkchain(data, buf, &j, a, len);
		while (j < len)
		{
			if (chain_delimeter(data, buf, &j))
				break;
			j++;
		}

		a = j + 1;
		if (a >= len)
		{
			a = len = 0;
			data->cmd_buf_type = CMD_NORM;
		}

		*buf_p = p;
		return (string_length(p));
	}

	*buf_p = buf;
	return (br);
}

/**
 * read_buffer - reads a buffer
 * @data: parameter struct
 * @buf: buffer
 * @i: size
 *
 * Return: bytes read
 */
ssize_t read_buffer(data_t *data, char *buf, size_t *i)
{
	ssize_t br = 0;

	if (*i)
		return (0);
	br = read(data->readfd, buf, READ_BUF_SIZE);
	if (br >= 0)
		*i = br;
	return (br);
}

/**
 * getline - gets the next line of input from STDIN
 * @data: parameter struct
 * @ptr: address of pointer to buffer, preallocated or NULL
 * @length: size of preallocated ptr buffer if not NULL
 *
 * Return: size of preallocated ptr buffer if not NULL
 */
int getline(data_t *data, char **ptr, size_t *length)
{
	static char buf[READ_BUF_SIZE];
	static size_t a, len;
	size_t k;
	ssize_t br = 0, s = 0;
	char *p = NULL, *new_p = NULL, *c;

	p = *ptr;
	if (p && length)
		s = *length;
	if (a == len)
		a = len = 0;

	br = read_buffer(data, buf, &len);
	if (br == -1 || (br == 0 && len == 0))
		return (-1);

	c = string_chr(buf + a, '\n');
	k = c ? 1 + (unsigned int)(c - buf) : len;
	new_p = realloc(p, s, s ? s + k : k + 1);
	if (!new_p) /* MALLOC FAILURE! */
		return (p ? free(p), -1 : -1);

	if (s)
		string_cat(new_p, buf + a, k - a);
	else
		copy_string(new_p, buf + a, k - a + 1);

	s += k - a;
	a = k;
	p = new_p;

	if (length)
		*length = s;
	*ptr = p;
	return (s);
}

/**
 * interruptHandler - handles the SIGINT signal (Ctrl-C)
 * @sig_num: the signal number
 *
 * Return: void
 */
void interruptHandler(__attribute__((unused))int sig_num)
{
	print_string("\n");
	print_string("$ ");
	write_character(BUF_FLUSH);
}
