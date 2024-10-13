/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 13:30:51 by jegoh             #+#    #+#             */
/*   Updated: 2023/09/26 22:21:26 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
read the text available on a file descriptor 1 line at a time until the EOF.
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c \
	get_next_line_utils.c; ./a.out; rm a.out;
*/
#include "get_next_line.h"

static char	*ft_fill_buffer_from_fd(int fd, t_gnl *gnl)
{
	gnl->rd_bytes = read(fd, gnl->buff, BUFFER_SIZE);
	if (gnl->rd_bytes == -1)
	{
		free(gnl->buff);
		gnl->buff = NULL;
		return (NULL);
	}
	gnl->buff[gnl->rd_bytes] = '\0';
	return (gnl->buff);
}

static char	*ft_accumulate_unprocessed_data(int fd, t_gnl *gnl)
{
	gnl->buff = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!gnl->buff)
		return (NULL);
	gnl->rd_bytes = 1;
	while (!ft_strchr(gnl->left_str, '\n') && gnl->rd_bytes != 0)
	{
		gnl->buff = ft_fill_buffer_from_fd(fd, gnl);
		if (!gnl->buff)
			return (NULL);
		if (gnl->left_str)
			gnl->left_str = ft_strjoin(gnl->left_str, gnl->buff);
		else
			gnl->left_str = ft_strdup(gnl->buff);
	}
	free(gnl->buff);
	gnl->buff = NULL;
	return (gnl->left_str);
}

static char	*ft_assemble_line_from_buffer(t_gnl *gnl)
{
	int		i;
	char	*str;

	i = 0;
	if (!gnl->left_str || !gnl->left_str[i])
		return (NULL);
	while (gnl->left_str[i] && gnl->left_str[i] != '\n')
		i++;
	str = (char *)malloc(sizeof(char) * (i + 2));
	if (!str)
		return (NULL);
	i = 0;
	while (gnl->left_str[i] && gnl->left_str[i] != '\n')
	{
		str[i] = gnl->left_str[i];
		i++;
	}
	if (gnl->left_str[i] == '\n')
	{
		str[i] = gnl->left_str[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

static char	*ft_generate_remaining_string(t_gnl *gnl)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	j = 0;
	if (!gnl->left_str)
		return (NULL);
	while (gnl->left_str[i] && gnl->left_str[i] != '\n')
		i++;
	if (!gnl->left_str[i])
	{
		free(gnl->left_str);
		return (NULL);
	}
	str = (char *)malloc(sizeof(char) * (ft_strlen(gnl->left_str) - i) + 1);
	if (!str)
		return (NULL);
	i++;
	while (gnl->left_str[i])
		str[j++] = gnl->left_str[i++];
	str[j] = '\0';
	free(gnl->left_str);
	gnl->left_str = str;
	return (str);
}

char	*get_next_line(int fd)
{
	static t_gnl	gnl;
	char			*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (0);
	gnl.left_str = ft_accumulate_unprocessed_data(fd, &gnl);
	if (!gnl.left_str)
		return (NULL);
	line = ft_assemble_line_from_buffer(&gnl);
	gnl.left_str = ft_generate_remaining_string(&gnl);
	return (line);
}
/*
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int	main(void)
{
	char	*line;
	int		i;
	int		fd1;
	int		fd2;
	int		fd3;
	fd1 = open("get_next_line.h", O_RDONLY);
	fd2 = open("get_next_line.c", O_RDONLY);
	fd3 = open("get_next_line_utils.c", O_RDONLY);
	i = 1;
	while ((line = get_next_line(fd1)))
	{
		if (line)
		{
    		printf("fd1 line [%02d]: %s", i, line);
    		free(line);
		}
		i++;
	}
	i = 1;
	while ((line = get_next_line(fd2)))
	{
		if (line)
		{
			printf("fd2 line [%02d]: %s", i, line);
			free(line);
		}
		i++;
	}
	i = 1;
	while ((line = get_next_line(fd2)))
	{
		if (line)
		{
			printf("fd3 line [%02d]: %s", i, line);
			free(line);
		}
		i++;
	}
	close(fd1);
	close(fd2);
	close(fd3);
	return (0);
}
*/
