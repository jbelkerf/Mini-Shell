/* ************************************************************************** */
/*                                                                            */
/*                                                         :::      ::::::::  */
/*   execute.c                                           :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*   By: Jbelkerf && isel-mou <minishell>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*   Created: 2025/06/20 13:02:27 by Jbelkerf && isel-mou #+#    #+#          */
/*   Updated: 2025/06/20 13:06:51 by Jbelkerf && isel-mou ###   ########.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../include/shell.h"

extern int	g_signal;

void	redirect(t_list *list, t_list *cmd, t_envp *envp)
{
	t_list	*current;
	char	**files;
	int		herdoc_visited;

	current = ((herdoc_visited = 0), cmd->redirs);
	while (current)
	{
		expand_files(current->next, &files, envp, current);
		if (check_ambiguous(current->next->value, files, list))
			break ;
		if (cmd->here_doc && current->type == HDOC && !herdoc_visited)
			herdoc_visited = ((ft_dup2(cmd->here_doc, 0)), 1);
		if (current->type == IN)
			ft_dup2(open_wrapper(files[0], O_RDONLY, 0), 0);
		if (current->type == OUT)
			ft_dup2(open_wrapper(files[0], O_W | O_C | O_TRUNC, 0666), 1);
		if (current->type == APP)
			ft_dup2(open_wrapper(files[0], O_W | O_C | O_APPEND, 0666), 1);
		ft_free_split(files);
		current = current->next->next;
	}
}

void	find_binary(char *PATH, char **cmd_path, char *cmd)
{
	int		i;
	char	**path;

	path = ft_split(PATH, ':');
	i = -1;
	while (path[++i])
	{
		*cmd_path = path[i];
		path[i] = (ft_strjoin(path[i], "/"));
		free(*cmd_path);
	}
	i = 0;
	*cmd_path = ft_strjoin(path[i], cmd);
	while (path[i] && *cmd_path && access(*cmd_path, X_OK))
		*cmd_path = ((free(*cmd_path)), ft_strjoin(path[i++], cmd));
	ft_free_split(path);
}

char	*get_cmd_path(char *cmd, t_envp *envp)
{
	char	*cmd_path;
	char	*tmp;

	if (ft_strchr(cmd, '/'))
	{
		if (cmd && !access(cmd, X_OK))
			return (cmd);
		return (NULL);
	}
	if (!ft_get_env_val(envp, "PATH"))
	{
		cmd_path = getcwd(NULL, 0);
		tmp = ft_strjoin(cmd_path, "/");
		free(cmd_path);
		cmd_path = ft_strjoin(tmp, cmd);
		free(tmp);
		return (cmd_path);
	}
	find_binary(ft_get_env_val(envp, "PATH"), &cmd_path, cmd);
	if (cmd_path && !access(cmd_path, X_OK))
		return (cmd_path);
	return (free(cmd_path), NULL);
}

void	execute_cmd(t_list *list, t_list *cmd, t_envp *envp, t_list *prev)
{
	char	**envp_char;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (prev)
	{
		ft_dup2(prev->pipe_fds[0], STDIN_FILENO);
		close(prev->pipe_fds[1]);
	}
	if (cmd->next)
	{
		ft_dup2(cmd->next->pipe_fds[1], STDOUT_FILENO);
		close(cmd->next->pipe_fds[0]);
	}
	if (cmd->redirected)
		redirect(list, cmd, envp);
	if (cmd->value == NULL)
		exit (0);
	envp_char = envp_to_char(envp);
	is_bin(cmd, envp);
	cmd->cmd_path = get_cmd_path(cmd->args[0], envp);
	if (cmd->cmd_path)
		execve(cmd->cmd_path, cmd->args, envp_char);
	exec_error(&cmd);
}

void	execute(t_list *list, t_list *current, t_envp *envp)
{
	t_list	*prev;
	int		status;
	int		is_built_in;

	prev = NULL;
	while (current)
	{
		execute_builtin(current, envp, prev, &is_built_in);
		if (!is_built_in && current->type == CMD)
		{
			current->pid = fork_wrapper(envp);
			if (current->pid == 0)
				execute_cmd(list, current, envp, prev);
			close_obsolete_fds(current, prev);
		}
		current = ((prev = current), current->next);
	}
	g_signal = ((signal(SIGINT, SIG_IGN)), (waitpid(prev->pid, &status, 0)), 0);
	if (WIFEXITED(status) && !is_built_in)
		envp->value = ((free(envp->value)), ft_itoa(WEXITSTATUS(status)));
	else if (WIFSIGNALED(status) && !is_built_in)
		envp->value = ((free(envp->value)), ft_itoa(WTERMSIG(status) + 128));
	while (wait(NULL) > 0)
		;
	signal(SIGINT, print_prompt);
}
