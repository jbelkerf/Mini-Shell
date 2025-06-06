#include "../include/shell.h"

#define FOUND_PWD     1
#define FOUND_OLDPWD  2 

void	redirect_builtins(t_list *current)
{
	int	stdout_fd;
	int	stdin_fd;

	if (!current->redirected)
		return ;
	stdin_fd = dup(STDIN_FILENO);
	stdout_fd = dup(STDOUT_FILENO);
	redirect(current);
	ft_dup2(stdout_fd, STDOUT_FILENO);
	ft_dup2(stdin_fd, STDIN_FILENO);
}

void	export(char **args, t_envp *envp, t_list *current, t_list *prev)
{
	int		i;
	t_envp	*node;
	int		in;
	int		out;

	in = dup(STDIN_FILENO);
	out = dup(STDOUT_FILENO);
	if (!prev && !current->next)
		redirect(current);
	i = 1;
	while (args[i])
	{
		if (!is_valid_export_argument(args[i]))
		{
			ft_dup2(in, STDIN_FILENO);
			ft_dup2(out, STDOUT_FILENO);
			return (identifier_error("export", args[i], envp));
		}
		free(envp->value);
		envp->value = ft_strdup("0");
		if (ft_strchr(args[i], '='))
		{
			*(ft_strchr(args[i], '=')) = '\0';
			envp = remove_envp_var(envp, args[i]);
			node = create_envp_node(args[i]);
			node->next = envp->next;
			envp->next = node;
		}
		i++;
	}
	if (i == 1)
	{
		envp = envp->next;
		while (envp)
		{
			if (envp->name && envp->value)
				printf("declare -x %s=\"%s\"\n", envp->name, envp->value);
			envp = envp->next;
		}
	}
	ft_dup2(in, STDIN_FILENO);
	ft_dup2(out, STDOUT_FILENO);
}

void	cd(char **args, t_envp	*envp, t_list *current, t_list *prev)
{
	struct stat	dir;
	char		*path;
	char		*old_pwd;
	int			vars_found;
	t_envp		*prev_envp;

	redirect_builtins(current);
	path = args[1];
	if (!path)
		path = "/";
	stat(path, &dir);
	if (!S_ISDIR(dir.st_mode))
	{
		chdir(path);
		free(envp->value);
		envp->value = ft_strdup("1");
		ft_putstr_fd("Minishell: cd: ", 2);
		perror(path);
		free(envp->value);
		envp->value = ft_strdup("1");
		return ;
	}
	free(envp->value);
	envp->value = ft_strdup("0");
	if ((prev && prev->type == PIPE)
		|| (current->next && current->next->type == PIPE))
		return ;
	old_pwd = get_cwd(envp);
	chdir(path);
	vars_found = 0;
	while (envp)
	{
		if (!strcmp(envp->name, "PWD"))
		{
			vars_found |= FOUND_PWD;
			free(envp->value);
			envp->value = getcwd(NULL, 0);
		}
		if (!strcmp(envp->name, "OLDPWD"))
		{
			vars_found |= FOUND_OLDPWD;
			free(envp->value);
			envp->value = ft_strdup(old_pwd);
		}
		prev_envp = envp;
		envp = envp->next;
	}
	if (!(vars_found & FOUND_OLDPWD))
	{
		path = ft_strjoin("OLDPWD=", old_pwd);
		*(ft_strchr(path, '=')) = '\0';
		prev_envp->next = create_envp_node(path);
		prev_envp = prev_envp->next;
		free(path);
	}
	free(old_pwd);
	if (!(vars_found & FOUND_PWD))
	{
		old_pwd = get_cwd(envp);
		path = ft_strjoin("PWD=", old_pwd);
		free(old_pwd);
		*(ft_strchr(path, '=')) = '\0';
		prev_envp->next = create_envp_node(path);
		free(path);
	}
}

void	unset(char **args, t_envp *envp, t_list *current, t_list *prev)
{
	int		i;

	redirect_builtins(current);
	i = 1;
	if (args[i] && !is_valid_unset_argument(args[i]))
		return (identifier_error("unset", args[i], envp));
	free(envp->value);
	envp->value = ft_strdup("0");
	if ((prev && prev->type == PIPE)
		|| (current->next && current->next->type == PIPE))
		return ;
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

void	exit_cmd(char **args, t_envp *envp, t_list *current, t_list *prev)
{
	int	subshell;

	redirect_builtins(current);
	subshell = ((ft_putendl_fd("exit", 1)), (prev && prev->type == PIPE)
			|| (current->next && current->next->type == PIPE));
	if (!args[1])
	{
		free(envp->value);
		envp->value = ft_strdup("0");
		if (!subshell)
			exit(0);
		return ;
	}
	if (!args[2])
	{
		if (is_numeric(args[1]))
			exit_with_status(envp, ft_atoi(args[1]) % 256, subshell);
		else
			exit_numeric_error(args[1], envp, subshell);
		return ;
	}
	if (is_numeric(args[1]))
		exit_too_many_args(envp);
	else
		exit_numeric_error(args[1], envp, subshell);
}
