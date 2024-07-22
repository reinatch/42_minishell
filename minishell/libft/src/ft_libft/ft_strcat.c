
#include "../../includes/libft.h"


char	*ft_strcat(char *s1, const char *s2)
{
	size_t i;
	size_t j;
	size_t k;

	i = ft_strlen(s1);
	j = ft_strlen(s2);
	k = i + j;
	j = 0;
	while (i <= k && s2[j] != '\0')
	{
		s1[i] = s2[j];
		i++;
		j++;
	}
	s1[i] = '\0';
	return (s1);
}