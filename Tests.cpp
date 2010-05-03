#include <stdlib.h> 
#include <string.h>
#include "hardware/gpio.h"

/* This is hackish testing code that mallocs and frees while turning on and off
   LEDs to allow timings to be measured
 */

void TestMalloc()
{
	vPortDisableInterruptsFromThumb();
	unsigned char j = 0;
	while (1)
	{
		j++;
		unsigned char *t = (unsigned char *)malloc(3000);
		if (t)
			memset(t, j, 3000);
		unsigned char *t2 = (unsigned char *)malloc(4000);
		if (t2)
			memset(t2, j*2, 4000);
		unsigned char *t3 = (unsigned char *)malloc(5000);
		if (t3)
			memset(t3, j*3, 5000);
		unsigned char *t4 = (unsigned char *)malloc(6000);
		if (t4)
			memset(t4, j*4, 6000);
		
		int good = 1;
		if (t)
			for (int k = 0; k < 3000; k++)
				if (t[k] != j)
				{
					good = 0;
					break;
				}
		vGpioSet(18,good);
		good = 1;
		if (t2)
			for (int k = 0; k < 4000; k++)
				if (t2[k] != j*2)
				{
					good = 0;
					break;
				}
		vGpioSet(20,good);
		good = 1;
		if (t3)
			for (int k = 0; k < 5000; k++)
				if (t3[k] != j*3)
				{
					good = 0;
					break;
				}
		vGpioSet(21,good);
		good = 1;
		if (t4)
			for (int k = 0; k < 6000; k++)
				if (t4[k] != j*4)
				{
					good = 0;
					break;
				}
		vGpioSet(23,good);

		for (volatile int i = 0; i < 500000; i++);

		if (t) free(t);
		if (t2) free(t2);
		if (t3) free(t3);
		if (t4) free(t4);

		vGpioSet(18,0);
		vGpioSet(20,0);
		vGpioSet(21,0);
		vGpioSet(23,0);

		for (volatile int i = 0; i < 500000; i++);
	}
}

void TestMalloc2()
{
	unsigned char **list;
	list = (unsigned char **)malloc(sizeof (unsigned char*) * 205);
	if (list == NULL)
	{
		vGpioSet(18,1);
		return;
	}

	while (1)
	{
		for (int i = 1; i < 205; i++)
		{
			list[i] = (unsigned char*)malloc(i);
			if (list[i] == NULL)
			{
				vGpioSet(20,1);
				if (i < 190)
					vGpioSet(21,1);
				if (i < 200)
					vGpioSet(23,1);
				return;
			}
			memset(list[i], i, i);
		}
		for (int i = 1; i < 205; i++)
			if (list[i])
				free(list[i]);

		for (volatile int i = 0; i < 500000; i++);
		vGpioToggle(20);
	}
}


/* This checks preinitialized RAM is correctly init'd/copied from flash by the
 * boot code */

static unsigned int numbers[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
static char text[] = {'0', '1', '2', '3', '4'};
static unsigned int numbers2[21];
static char text2[6]; 

void TestInitializedData()
{
	while (1)
	{
		/* Test pre-initialized data */
		int good = 1;
		for (unsigned int k = 0; k < 21; k++)
			if (numbers[k] != k)
				good = 0;
		vGpioSet(18, good);

		for (unsigned int k = 0; k < 5; k++)
			if (text[k] != k + '0')
				good = 0;
		vGpioSet(21, good);

		/* Check uninitialized data */
		memcpy(numbers2, numbers, 21 * sizeof(unsigned int));
		memcpy(text2, text, 6);
		good = 1;
		for (unsigned int k = 0; k < 21; k++)
			if (numbers[k] != k)
				good = 0;
		vGpioSet(18, good);

		for (unsigned int k = 0; k < 5; k++)
			if (text[k] != k + '0')
				good = 0;
		vGpioSet(21, good);

		for (volatile int i = 0; i < 500000; i++);
		vGpioToggle(20);
	}
}

