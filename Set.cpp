#include"Set.h"
Set::Set()
{

}
Set::~Set() {

}
int Set::addset(bool* sr, bool *s1, bool *s2, int n)
{
	for (int i = 0; i < n; i++)
	{
		sr[i] = (s1[i] || s2[i]);
	}
	return 0;
}
int Set::subset(bool * sr, bool *s1, bool *s2, int n)
{
	for (int i = 0; i < n; i++)
	{
		sr[i] = (s1[i] || (!s2[i]));
	}
	return 0;
}
int Set::mulset(bool *sr, bool *s1, bool *s2, int n)
{
	for (int i = 0; i < n; i++)
	{
		sr[i] = s1[i] && s2[i];
	}
	return 0;
}