/* ************************************************************************** */
/*                                                                            */
/*                                                         :::      ::::::::  */
/*   builtins_bonus.c                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*   By: Jbelkerf && isel-mou <minishell>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*   Created: 2025/06/20 13:02:27 by Jbelkerf && isel-mou #+#    #+#          */
/*   Updated: 2025/06/20 13:06:51 by Jbelkerf && isel-mou ###   ########.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../include/shell_bonus.h"

void	unset(char **args, t_envp *envp, t_list *current, t_list *prev)
{
	int		i;

	redirect_builtins(current, envp);
	i = 1;
	free(envp->value);
	envp->value = ft_strdup("0");
	if (args[i] && !is_valid_unset_argument(args[i]))
		return (identifier_error("unset", args[i], envp));
	if (!prev && !current->next)
	{
		while (args[i])
		{
			if (!ft_strcmp(args[i], "?"))
			{
				i++;
				continue ;
			}
			remove_envp_var(envp, args[i]);
			i++;
		}
	}
	else if (current->next)
		close(current->next->pipe_fds[1]);
	if (prev)
		close(prev->pipe_fds[0]);
}

int	dash_n(char *str)
{
	if (*str == '\0')
		return (0);
	if (*str == '-')
	{
		str++;
		while (*str == 'n')
			str++;
	}
	if (*str == '\0')
		return (1);
	return (0);
}

void	echo_cmd(char **argv)
{
	int	i;

	i = 1;
	if (dash_n(argv[1]))
		i = 2;
	while (argv[i])
	{
		printf("%s", argv[i]);
		if (argv[i + 1])
			printf(" ");
		i++;
	}
	if (!dash_n(argv[1]))
		printf("\n");
}

void	env_cmd(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
		i++;
	while (--i > 0)
		printf("%s\n", envp[i]);
	ft_free_split(envp);
}

char	*get_cwd(t_envp *envp)
{
	while (envp)
	{
		if (!ft_strcmp(envp->name, "PWD"))
			return (ft_strdup(envp->value));
		envp = envp->next;
	}
	return (getcwd(NULL, 0));
}
