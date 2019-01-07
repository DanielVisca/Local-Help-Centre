#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256

/*
 * Return a pointer to the struct student with name stu_name
 * or NULL if no student with this name exists in the stu_list
 */
Student *find_student(Student *stu_list, char *student_name) {
    Student *curr = stu_list;
    while (curr != NULL) {
      if (strcmp(curr->name, student_name) == 0) {
        return curr;
      }
      curr = curr->next_overall;
    }
    return NULL;
  }



/*   Return a pointer to the ta with name ta_name or NULL
 *   if no such TA exists in ta_list. 
 */
Ta *find_ta(Ta *ta_list, char *ta_name) {
    Ta *curr = ta_list;
    while (curr != NULL) {
      if (strcmp(curr->name, ta_name) == 0) {
        return curr;
      }
      curr = curr->next;
    }
    return NULL;
}


/*  Return a pointer to the course with this code in the course list
 *  or NULL if there is no course in the list with this code.
 */
Course *find_course(Course *courses, int num_courses, char *course_code) {
    
    for(int i = 0; i < num_courses; i++) {
	
	if (strcmp(courses[i].code, course_code) == 0) {
	    Course *correct_course = &courses[i];
	    return correct_course;
	}
    }
    return NULL;
}
    

/* Add a student to the queue with student_name and a question about course_code.
 * if a student with this name already has a question in the queue (for any
   course), return 1 and do not create the student.
 * If course_code does not exist in the list, return 2 and do not create
 * the student struct.
 * For the purposes of this assignment, don't check anything about the 
 * uniqueness of the name. 
 */
int add_student(Student **stu_list_ptr, char *student_name, char *course_code,	
    Course *course_array, int num_courses) {
    // Check if course exists
    Course *found_course = find_course(course_array, num_courses, course_code);
    if (!found_course) { return 2; }
    
    // Check if student isnt in stu_list
    Student *found_student = find_student(*stu_list_ptr, student_name);
    
    if (found_student) { return 1; }

    Student *new_student = malloc(sizeof(Student));
    if (new_student == NULL) {
	    perror("Malloc for student");
	    exit(1);
    }
    new_student->name = malloc(strlen(student_name) + 1);
    if (new_student->name == NULL) {
	    perror("Malloc for student name");
	    exit(1);
    }
    new_student->course = malloc(sizeof(Course));
    if (new_student->course == NULL) {
    	perror("Malloc for student course");
	    exit(1);
    }
    new_student->course = found_course;
    strcpy(new_student->name, student_name);
    //new_student->next_overall = *stu_list_ptr;
    new_student->next_course = NULL;
    //new_student->arrival_time = time_t;
    time_t arrived_at;
    time(&arrived_at);
    new_student->arrival_time = &arrived_at;
    //traverse next_overall, add student in the end
    Student *last_in_course = NULL;
    
    Student *curr_overall = *stu_list_ptr;
     
    // first student added
   
    new_student->next_overall = *stu_list_ptr;
      
    while (curr_overall != NULL ) {
      
      if (strcmp(curr_overall->course->code, course_code) == 0) {
	      last_in_course = curr_overall;
	      break;
	    }
	    curr_overall = curr_overall->next_overall;
            
    }
    if (curr_overall == NULL) { curr_overall = new_student;  }  
    new_student->next_overall = *stu_list_ptr;
    
    *stu_list_ptr = new_student;
    // curr_overall->next_overall = new_student;
    if (last_in_course != NULL) {
	    new_student->next_course = last_in_course;
    }
    return 0; 
}


/* Student student_name has given up waiting and left the help centre
 * before being called by a Ta. Record the appropriate statistics, remove
 * the student from the queues and clean up any no-longer-needed memory.
 *
 * If there is no student by this name in the stu_list, return 1.
 */
int give_up_waiting(Student **stu_list_ptr, char *student_name) {
  Student *gave_up = find_student(*stu_list_ptr, student_name);
  if (!gave_up) {
    return 1;
  }
  //record stats here
  time_t the_time;
  time(&the_time);
  gave_up->course->wait_time += (&the_time - gave_up->arrival_time);  
  if (strcmp((*stu_list_ptr)->name, student_name) == 0){
    Student *temp = *stu_list_ptr;
    *stu_list_ptr = ((*stu_list_ptr)->next_overall);
    free(temp);
    return 0;
  }

  Student *curr = (*stu_list_ptr)->next_overall;
  Student *previous = *stu_list_ptr;

  while (curr != NULL && previous != NULL) {
    if (strcmp(curr->next_overall->name, student_name) == 0 ) {
      Student *temp = curr;
      previous->next_overall = curr->next_overall;
      free(temp);
      return 0; 
      }
    previous = curr;
    curr = curr->next_overall;
  }

  free(gave_up);
  return 0;
}

/* Create and prepend Ta with ta_name to the head of ta_list. 
 * For the purposes of this assignment, assume that ta_name is unique
 * to the help centre and don't check it.
 */
void add_ta(Ta **ta_list_ptr, char *ta_name) {
    // first create the new Ta struct and populate
    Ta *new_ta = malloc(sizeof(Ta));
    if (new_ta == NULL) {
       perror("malloc for TA");
       exit(1);
    }
    new_ta->name = malloc(strlen(ta_name)+1);
    if (new_ta->name  == NULL) {
       perror("malloc for TA name");
       exit(1);
    }
    strcpy(new_ta->name, ta_name);
    new_ta->current_student = NULL;

    // insert into front of list
    new_ta->next = *ta_list_ptr;
    *ta_list_ptr = new_ta;
}

/* The TA ta is done with their current student. 
 * Calculate the stats (the times etc.) and then 
 * free the memory for the student. 
 * If the TA has no current student, do nothing.
 */
void release_current_student(Ta *ta) {
  if (ta->current_student != NULL) {
    ta->current_student->course->helped++;
    //questions answered += 1
    
    

    //Course ->wait time = time helped at - arrival time
    
    //Course->help_time = current time - time helped at

    //remove from stu_list
    free(ta->current_student);
  } 
}

/* Remove this Ta from the ta_list and free the associated memory with
 * both the Ta we are removing and the current student (if any).
 * Return 0 on success or 1 if this ta_name is not found in the list
 */
int remove_ta(Ta **ta_list_ptr, char *ta_name) {
    Ta *head = *ta_list_ptr;
    if (head == NULL) {
        return 1;
    } else if (strcmp(head->name, ta_name) == 0) {
        // TA is at the head so special case
        *ta_list_ptr = head->next;
        release_current_student(head);
        // memory for the student has been freed. Now free memory for the TA.
        free(head->name);
        free(head);
        return 0;
    }
    while (head->next != NULL) {
        if (strcmp(head->next->name, ta_name) == 0) {
            Ta *ta_tofree = head->next;
            //  We have found the ta to remove, but before we do that 
            //  we need to finish with the student and free the student.
            //  You need to complete this helper function
            release_current_student(ta_tofree);

            head->next = head->next->next;
            // memory for the student has been freed. Now free memory for the TA.
            free(ta_tofree->name);
            free(ta_tofree);
            return 0;
        }
        head = head->next;
    }
    // if we reach here, the ta_name was not in the list
    return 1;
}

void update_course_stats(Course *course, Student *student) {
  time_t the_time;
  time(&the_time);

  course->helped += 1;
  
  time_t waited;

  waited = the_time - (*student->arrival_time);
  course->wait_time += waited;
  time_t helped;
  helped = the_time - waited;
  course->help_time += helped;
  //current_time;
  
  //helped
  //course->help_time
  //course->wait_time = student->arrived - current_time;
  //wait time
  //help time
}



/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the full queue. 
 * If the queue is empty, then TA ta_name simply finishes with the student 
 * they are currently helping, records appropriate statistics, 
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 */
int take_next_overall(char *ta_name, Ta *ta_list, Student **stu_list_ptr) {
    Ta *ta_found = find_ta(ta_list, ta_name);
    if (!ta_found) { return 1; }
    Student *curr = *stu_list_ptr;
    while (curr != NULL) {
      if(curr->next_overall == NULL) {
        break;
      }
      curr = curr->next_overall;
    }

    // if ta has current student, record stats
    if (ta_found->current_student != NULL) {
      update_course_stats(ta_found->current_student->course, ta_found->current_student);
      stu_list_ptr = &(ta_found->current_student->next_overall);
      ta_found->current_student = NULL;
    }

    ta_found->current_student = curr;
    stu_list_ptr = &(ta_found->current_student->next_overall);
  
  return 0;
}



/* TA ta_name is finished with the student they are currently helping (if any)
 * and are assigned to the next student in the course with this course_code. 
 * If no student is waiting for this course, then TA ta_name simply finishes 
 * with the student they are currently helping, records appropriate statistics,
 * and sets current_student for this TA to NULL.
 * If ta_name is not in ta_list, return 1 and do nothing.
 * If course is invalid return 2, but finish with any current student. 
 */
int take_next_course(char *ta_name, Ta *ta_list, Student **stu_list_ptr, char *course_code, Course *courses, int num_courses) {
    
    return 0;
}


/* For each course (in the same order as in the config file), print
 * the <course code>: <number of students waiting> "in queue\n" followed by
 * one line per student waiting with the format "\t%s\n" (tab name newline)
 * Uncomment and use the printf statements below. Only change the variable
 * names.
 */
void print_all_queues(Student *stu_list, Course *courses, int num_courses) {
  // might have to reverse, no biggy
  for (int i = 0; i < num_courses; i++) {
    Course curr = courses[i];
    //traverse students and return the number of students
    int stu_count = 0;
    //find first student in the list that is enrolled in this course
    Student *counter = stu_list;
    Student *first_in_course = NULL;
    while (counter != NULL) {
      //compare current course to the course of the student
      if(strcmp(curr.code, counter->course->code)==0){
        first_in_course = counter;
        break;
      }
      counter = counter->next_overall;
    }
  
    Student *count_curr_stu = first_in_course;
    while (count_curr_stu != NULL) {
        stu_count++;
        count_curr_stu = count_curr_stu->next_course;
    }
    printf("%s: %d in queue\n", curr.code, stu_count); 

    Student *curr_stu = first_in_course;
    while (curr_stu != NULL) {
        printf("\t%s\n",curr_stu->name); 
        curr_stu = curr_stu->next_course;
    }
  }
}


/*
 * Print to stdout, a list of each TA, who they are serving at from what course
 * Uncomment and use the printf statements 
 */
void print_currently_serving(Ta *ta_list) {
  if (ta_list == NULL) {
    printf("No TAs are in the help centre.\n"); 
  }
  else {
    Ta *curr = ta_list;
    while (curr != NULL) {
      if (curr->current_student == NULL) {
        printf("TA: %s has no student\n", curr->name);
        curr = curr->next; 
      }
      else {

        printf("TA: %s is serving %s from %s\n", curr->name, curr->current_student->name, curr->current_student->course->code);
        curr = curr->next; 
      }
    }

  }
 
    //printf("No TAs are in the help centre.\n");
    //printf("TA: %s is serving %s from %s\n",i var1, var2);
    //printf("TA: %s has no student\n", var3);
}


/*  list all students in queue (for testing and debugging)
 *   maybe suggest it is useful for debugging but not included in marking? 
 */ 
void print_full_queue(Student *stu_list) {

}

/* Prints statistics to stdout for course with this course_code
 * See example output from assignment handout for formatting.
 *
 */
int stats_by_course(Student *stu_list, char *course_code, Course *courses, int num_courses, Ta *ta_list) {
 
    // TODO: students will complete these next pieces but not all of this 
    //       function since we want to provide the formatting
    
    Course *found = find_course(courses, num_courses, course_code); 
    // students_being_helped (number of students from this course that are currently being helped
    //	go through TA list and find all of the students currently being helped, if student course == course. Add 1 to the counter. 
 
    if (found) {
   
        int students_being_helped = 0;
        while (ta_list != NULL) {
	    
	    if (strcmp(((ta_list->current_student)->course)->code, course_code) == 0) {
	        students_being_helped ++;
	    } 
	    ta_list = ta_list->next;
        }

        // students_waiting (number of students from this course that are currently waiting)
        //	 count from head, check next_course, until == NULL or == tail
        int students_waiting = 0;
	Student *current = found->head; 
	while(current != NULL){
            students_waiting++;
	    current = current->next_course;
	}
    }
    else {return 1;}
    // You MUST not change the following statements or your code 
    //  will fail the testing. 
/*
    printf("%s:%s \n", found->code, found->description);
    printf("\t%d: waiting\n", students_waiting);
    printf("\t%d: being helped currently\n", students_being_helped);
    printf("\t%d: already helped\n", found->helped);
    printf("\t%d: gave_up\n", found->bailed);
    printf("\t%f: total time waiting\n", found->wait_time);
    printf("\t%f: total time helping\n", found->help_time);
*/
    return 0;
}


/* Dynamically allocate space for the array course list and populate it
 * according to information in the configuration file config_filename
 * Return the number of courses in the array.
 * If the configuration file can not be opened, call perror() and exit.
 */
int config_course_list(Course **courselist_ptr, char *config_filename) {
    FILE *config_file = fopen(config_filename, "r");
    // exit if file wont open
    if (config_file == NULL) {
	    printf("%s", config_filename);
	    perror("Error: config_file cannot open");
	    exit(1);
    }
    char firstline[INPUT_BUFFER_SIZE + 1];
    char *excess[INPUT_BUFFER_SIZE + 1];
    char line[INPUT_BUFFER_SIZE + 1];

    
    fgets( firstline, INPUT_BUFFER_SIZE + 1, config_file); 
    long *size = malloc(sizeof(int)); 
    *size = strtol(firstline, excess, 10);

    // allocate space for the course list array 
    *courselist_ptr = malloc(sizeof(Course) * (*size));
    int i = 0;
    while(fgets(line, INPUT_BUFFER_SIZE + 1, config_file) != NULL ) {
	    
	    //populate struct
	    	   
            strncpy((*courselist_ptr)[i].code, line, (sizeof(char) * 6) );
            (*courselist_ptr)[i].code[7] = '\0';


	    (*courselist_ptr)[i].description = line + 6;
	  
	    // null terminator added by fgets
	    
	    i++;
    }
    
    // exit if file wont close
    if(fclose(config_file) != 0) {
	    perror("Error: config_file cannot open");
	    exit(1);
    }
    return *size;
}



