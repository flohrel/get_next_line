/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flohrel <flohrel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/02 10:17:42 by flohrel           #+#    #+#             */
/*   Updated: 2021/01/11 20:19:16 by flohrel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	set_line(t_queue *file_q, char **line)
{
	char	*lptr;
	t_buf	*cur_buf;

	*line = malloc(sizeof(char) * (file_q->len + 1));
	if (!(*line))
		return (-1);
	lptr = *line;
	while ((cur_buf = file_q->first))
	{
		ft_memcpy(lptr, cur_buf->data, cur_buf->size);
		lptr += cur_buf->size;
		pop(file_q, 0);
	}
	*lptr = '\0';
	file_q = init_file_queue(file_q);
	return (0);
}

int	read_tmp(t_queue *file_q, char **line)
{
	char	*c;
	t_buf	*tmp;
	size_t	size;
	size_t	tmp_size;

	tmp = file_q->tmp;
	c = ft_strchr(tmp->data, '\n', tmp->size);
	if (!c)
	{
		if (push(file_q, tmp->data, tmp->size, 0) == -1)
			return (-1);
		pop(file_q, 1);
		return (0);
	}
	size = c - tmp->data;
	tmp_size = tmp->size - size - 1;
	if (size && push(file_q, tmp->data, size, 0) == -1)
		return (-1);
	if ((set_line(file_q, line) == -1) ||
		(tmp_size && push(file_q, c + 1, tmp_size, 1) == -1))
		return (-1);
	free(tmp->data);
	free(tmp);
	return (1);
}

int	read_fd(t_queue *file_q, int fd, char **line)
{
	char	*c;
	char	buf[BUFFER_SIZE];
	int		ret;
	size_t	size;
	size_t	tmp_size;

	while ((ret = read(fd, buf, BUFFER_SIZE)) > 0)
	{
		c = ft_strchr(buf, '\n', ret);
		if (c)
		{
			size = c - buf;
			tmp_size = ret - size - 1;
			if ((size && (push(file_q, buf, size, 0) == -1)) ||
				(set_line(file_q, line) == -1) ||
				(tmp_size && push(file_q, c + 1, tmp_size, 1) == -1))
				return (-1);
			return (1);
		}
		if (push(file_q, buf, ret, 0) == -1)
			return (-1);
	}
	return (ret);
}

int	get_next_line(int fd, char **line)
{
	static t_queue	*qlist[MAX_FD] = { NULL };
	t_queue			*file_q;
	int				ret;

	if (!line || fd < 0 || BUFFER_SIZE <= 0)
		return (-1);
	if (!qlist[fd] && !(qlist[fd] = init_file_queue(qlist[fd])))
		return (-1);
	file_q = qlist[fd];
	ret = 0;
	if (file_q->tmp)
		ret = read_tmp(file_q, line);
	if (!ret)
		ret = read_fd(file_q, fd, line);
	if (!ret)
	{
		set_line(file_q, line);
		free(file_q);
		qlist[fd] = NULL;
	}
	return (ret);
}
