/* ************************************************************************** */
/*                                                                            */
/*                                                         :::      ::::::::  */
/*   builtins_utils.c                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*   By: Jbelkerf && isel-mou <minishell>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*   Created: 2025/06/20 13:02:27 by Jbelkerf && isel-mou #+#    #+#          */
/*   Updated: 2025/06/20 13:06:51 by Jbelkerf && isel-mou ###   ########.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../include/shell.h"

void	redirect_builtins(t_list *current, t_envp *envp)
{
	int	stdout_fd;
	int	stdin_fd;

	if (!current->redirected)
		return ;
	stdin_fd = dup(STDIN_FILENO);
	stdout_fd = dup(STDOUT_FILENO);
	redirect(NULL, current, envp);
	ft_dup2(stdout_fd, STDOUT_FILENO);
	ft_dup2(stdin_fd, STDIN_FILENO);
}

void	handle_simple_builtins(t_list *cmd, t_envp *envp)
{
	char	*cwd;

	if (!ft_strcmp(cmd->value, "echo") || !ft_strcmp(cmd->value, "ECHO"))
	{
		if (!cmd->args[1])
		{
			write(1, "\n", 1);
			exit(0);
		}
		echo_cmd(cmd->args);
		exit(0);
	}
	else if (!ft_strcmp(cmd->value, "env"))
	{
		env_cmd(envp_to_char(envp));
		exit(0);
	}
	else if (!ft_strcmp(cmd->value, "pwd") || !ft_strcmp(cmd->value, "PWD"))
	{
		cwd = get_cwd(envp);
		printf("%s\n", cwd);
		free(cwd);
		exit(0);
	}
}

void	is_bin(t_list *cmd, t_envp *envp)
{
	handle_simple_builtins(cmd, envp);
	if (!ft_strcmp(cmd->value, "export"))
	{
		export(cmd->args, envp, cmd, NULL);
		exit (0);
	}
}

int	is_valid_unset_argument(char *arg)
{
	int	i;

	i = 0;
	if (!ft_isalpha(arg[i]) && arg[i] != '_')
		return (0);
	while (arg[i])
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	is_numeric(char *arg)
{
	int		i;
	int		sign;
	long	res;

	sign = ((i = 0), 1);
	if (arg[i] == '+' || arg[i] == '-')
	{
		if (arg[i] == '-')
			sign = -1;
		i++;
		if (!ft_isdigit(arg[i]))
			return (0);
	}
	res = 0;
	while (arg[i])
	{
		if (!ft_isdigit(arg[i]))
			return (0);
		if (sign == 1 && (res * 10 + (arg[i] - '0')) * sign < res * sign)
			return (0);
		if (sign == -1 && (res * 10 + (arg[i] - '0')) * sign > res * sign)
			return (0);
		res = ((i++), res * 10 + (arg[i] - '0'));
	}
	return (i);
}
