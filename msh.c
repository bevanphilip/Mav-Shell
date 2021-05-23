/*
  Name: Bevan Philip
  ID: 1001527799
*/

// The MIT License (MIT)
//
// Copyright (c) 2016, 2017, 2020 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
// so we need to define what delimits our tokens.
// In this case  white space
// will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

int main()
{
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  //stores all user input except exit,quit,!n
  //it is a double pointer so that we can malloc each index of array and store
  //user input
  char ** history=  malloc(sizeof(char *) * 15);
  //stores 15 pid IDs
  int *pid_array = (int*) malloc(sizeof(int) * 15);
  //start pid history array from index zero and make it global go that  we can
  //track the actual index while printing
  int index=0;
  //starts history array from index zero, make it global variable to track the actual
  //index from an array
  int hist_index=0;
  //checks to see if history array is full or not
  int indicator =0;


  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input

    //removed semicolon due to error Hw1.c:36:55: warning: while loop has empty body [-Wempty-body]
    //moved semicolon to next line to avoid seg fault
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) )
    ;
    //handles enter as user input
    if(cmd_str[0]=='\n')
    {
      continue;
    }

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int  token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_str  = strdup( cmd_str );

    //saves user input here if input is not enter,exit,quit, and !n
    if((cmd_str[0]!= '\n') && (cmd_str[0]!='e') && (cmd_str[0]!='q') &&(cmd_str[0]!='!'))
    {  //malloc each index in history array to store history
      //the size of each index depends on the command entered by the user
      history[hist_index] = malloc(strlen(cmd_str));
      strncpy(history[hist_index],cmd_str,strlen(cmd_str));
      hist_index++;
      if(hist_index>14)
      {
        hist_index=0;
        indicator =1;
      }
    }

    //if user input is !n, it strtoks input and checks for the n index
    //in history array
    if(cmd_str[0]=='!')
    {
      char *c=strtok(&cmd_str[1],"\n");
      int a= atoi(c);
      int num =hist_index;
      //prevents from segfaulting if user types higer value than the current
      //highest index in array
      if((a >= (num) && a < 15 && indicator!=1 ))
      {
        printf("out of bounds\n");
        continue;
      }
      //handles input higer than 15 or less than zero
      else if(a >= 15 || a < 0)
      {
        printf("Command not in history\n");
        continue;
      }
      //if given index is valid then copy that command from history
      //into working_str
      else
      {
        strcpy(working_str,history[a]);
      }
    }

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
    (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen(token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }

      token_count++;
    }

    //if input is quit or exit it exits the program
    if(strcmp(token[0],"exit")==0 || strcmp(token[0],"quit")==0)
    {
      printf("exiting\n");
      exit(0);
    }
    //if input is cd then change directory
    else if(strcmp(token[0],"cd")==0)
    {
      int hold;
      hold=chdir(token[1]);
      //if return value is less than 0 that means directory directory
      //does not exist
      if(hold<0)
      {
        printf("Directory does not exist\n");
      }

    }
    //if input is showpids then print all pid values
    else if(strcmp(token[0],"showpids")==0)
    {
      //if array has more than 15 elements current will store
      //index number
      int current =0;
      //variables which help to loop
      int i,j,k;
      //counts number of elements in an array
      int counter=0;

      //count total number of elemnts in an array
      for(i=0; i<15;i++)
      {
        if(pid_array[i]!=0)
        {
          counter++;
        }
      }
      //if array has less than 15 elements
      if(counter<15)
      {
        for (j=0; j<counter;j++)
        {
          printf("%d: %d\n",j,pid_array[j]);
        }
      }
      //if array has more than 15 elements
      else
      {
        current = index;
        for (k =0; k<15;k++)
        {
          printf("%d %d\n",k,pid_array[current]);
          current++;
          if(current > 14)
          {
            current = 0;
          }
        }
      }
    }
    //if input is history
    else if(strcmp(token[0],"history")==0)
    {
      //if array has more than 15 elements current will store
      //index number
      int counter=0;
      //variables which help to loop
      int i,j,k,l;
      //count number of elements in history array
      for(i=0;i<15;i++)
      {
        if(history[i]!=0)
        {
          counter++;
        }
      }
      //if array has less than 15 elements
      if(counter<15)
      {
        for(j=0; j<counter;j++)
        {
          printf("%d: %s",j,history[j]);
        }
      }
      //if array has more than 15 elements
      else
      {
        k=hist_index;
        for(l=0; l<15;l++)
        {
          printf("%d: %s",l,history[k]);
          k++;
          if(k>14)
          {
            k=0;
          }

        }
      }

    }
    //handles all other input other than checked condtion above
    else
    { //system call which returns a copy parent process
      pid_t pid =fork();
      if(pid == 0)
      {
        //replaces all the code in child with new given code and
        //makes it a new process
        int ret = execvp(token[0],&token[0]);
        //executes if input is invalid command
        if(ret== -1)
        {
          printf("%s: command not found\n",token[0]);
          exit(0);
        }
        else
        {
          printf("success\n");
        }
      }
      //error occured during fork call
      else if(pid<0)
      {
        printf("fork call unsuccesful..\n");
      }
      //store pid values in parent because child will always be zero
      else
      {
        //pid IDs are stored in pid array which helps us to print pid IDs
        pid_array[index++] = pid;
        if(index>14)
        {
          index = 0;
        }

        int status;
        wait(& status);
      }
    }
    free( working_root );
  }
  return 0;
}
