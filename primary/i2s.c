void i2s(int i,char *s)	// Convert Integer to String
{
	char sign;
	short len;
	char *p;
	sign = '+';
	len = 0;
	p = s;
	if(i < 0){
		sign='-';
		i=-i;
	}
	do {
		*s=(i%10)+'0';
		s++;
		len++;
		i /= 10;
	} while (i != 0);
	if(sign == '-'){
		*s = '-';
		s++;
		len++;
	}
	for(i = 0; i < len/2; i++){
		p[len] = p[i];
		p[i] = p[len-1-i];
		p[len-1-i] = p[len];
	}
	p[len] = 0;
}
