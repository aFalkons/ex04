 #include <unistd.h>
 #include <sys/wait.h>
 #include <string.h>

int	err(char *s, char *a)
{
	 while (*s)
		 write(2, s++, 1);
	 if (a)
		 while (*a)
			 write(2, a++, 1);
	 write(2, "\n", 1);
	 return (1);
}

int	exec(char **a, int i, int tmp, char **env)
{
	a[i] = NULL;
	dup2(tmp, 0);
	close(tmp);
	execve(a[0], a, env);
	return (err("error: cannot execute ", a[0]));
}

int    main(int aa, char **a, char **env)
{
	int	i = 0;
	int	pid = 0;
	int	tmp = dup(0);
	int	fd[2];
	(void)aa;
	while (a[i] && a[i + 1])
	{
		a = &a[i + 1];
		i =  0;
		while (a[i] && strcmp(a[i], "|") && strcmp(a[i], ";"))
			 i++;
		if (!strcmp(a[0], "cd"))
		{
			 if (i != 2)
				 err("error: cd: bad arguments", NULL);
			 else if (chdir(a[1]) != 0)
				 err("error: cd: cannot change directory to ", a[1]);
		}
		else if (i != 0 && (a[i] == NULL || !strcmp(a[i], ";")))
		{
			pid = fork();
			if (!pid)
			{
				if (exec(a, i, tmp, env))
					return (1);
			}
			else
			{
				close(tmp);
				while (waitpid(-1, NULL, WUNTRACED) != -1)
					;
				tmp = dup(0);
			}
		}
		else if (i != 0 && !strcmp(a[i], "|"))
		{
			pipe(fd);
			pid = fork();
			if (!pid)
			{
				dup2(fd[1], 1);
				close(fd[1]);
				close(fd[0]);
				if (exec(a, i, tmp, env))
					return (1);
			}
			else
			{
				close(tmp);
				close(fd[1]);
				tmp = fd[0];
			}
		}
	}
	close (tmp);
	return (0);
}
