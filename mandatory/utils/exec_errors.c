/* ************************************************************************** */
/*                                                                            */
/*                                                         :::      ::::::::  */
/*   exec_errors.c                                       :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*   By: Jbelkerf && isel-mou <minishell>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*   Created: 2025/06/20 13:02:27 by Jbelkerf && isel-mou #+#    #+#          */
/*   Updated: 2025/06/20 13:06:51 by Jbelkerf && isel-mou ###   ########.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../include/shell.h"

void	is_directory_error(t_list *cmd)
{
	struct stat	buf;

	if (stat(cmd->args[0], &buf) == 0
		&& ft_strchr(cmd->args[0], '/')
		&& ft_strcmp(cmd->args[0], ".")
		&& ft_strcmp(cmd->args[0], "..")
		&& S_ISDIR(buf.st_mode))
	{
		ft_putstr_fd("Minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": is a directory\n", 2);
		free_list(cmd);
		exit(126);
	}
}

void	no_such_file_error(t_list *cmd)
{
	if ((cmd->cmd_path && access(cmd->cmd_path, F_OK) != 0)
		|| (cmd->args[0] && ft_strchr(cmd->args[0], '/')
			&& access(cmd->args[0], F_OK)))
	{
		ft_putstr_fd("Minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": No such file or directory\n", 2);
		free_list(cmd);
		exit(127);
	}
}

void	permission_denied_error(t_list *cmd)
{
	if ((cmd->cmd_path && !access(cmd->cmd_path, F_OK)
			&& access(cmd->cmd_path, X_OK)) || (ft_strchr(cmd->args[0], '/')
			&& !access(cmd->args[0], F_OK)
			&& access(cmd->args[0], X_OK)))
	{
		ft_putstr_fd("Minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": Permission denied\n", 2);
		free_list(cmd);
		exit(126);
	}
}

void	command_not_found_error(t_list *cmd)
{
	ft_putstr_fd("Minishell: ", 2);
	ft_putstr_fd(cmd->args[0], 2);
	ft_putstr_fd(": command not found\n", 2);
	free_list(cmd);
	exit(127);
}
