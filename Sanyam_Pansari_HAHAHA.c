/*
================================================================================
          COMPREHENSIVE STUDENT MANAGEMENT AND ANALYTICS SYSTEM
================================================================================

Purpose: A full-featured educational management system that handles student
         records, course management, grade tracking, and comprehensive analytics.

Author: Development Team
Date: November 2025
Version: 2.0

Features:
  - Student registration and profile management
  - Course enrollment and management
  - Grade tracking and calculation
  - Performance analytics and reporting
  - Data persistence (file I/O)
  - Search and filter capabilities
  - Statistical analysis
  - Export and import functionality

================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

/* ============================================================================
   CONSTANTS AND DEFINITIONS
   ============================================================================ */

#define MAX_STUDENTS 500
#define MAX_COURSES 100
#define MAX_ENROLLMENTS 5000
#define MAX_NAME_LENGTH 100
#define MAX_COURSE_CODE 20
#define MAX_DESCRIPTION 500
#define MAX_LOG_ENTRIES 10000
#define FILE_BUFFER_SIZE 4096
#define MAX_GRADES 20
#define MIN_GPA 0.0f
#define MAX_GPA 4.0f
#define MIN_GRADE 0
#define MAX_GRADE 100

/* Grade boundaries */
#define GRADE_A_MIN 90
#define GRADE_B_MIN 80
#define GRADE_C_MIN 70
#define GRADE_D_MIN 60
#define GRADE_F_MIN 0

/* Log levels */
#define LOG_INFO 1
#define LOG_WARNING 2
#define LOG_ERROR 3
#define LOG_SUCCESS 4

/* ============================================================================
   DATA STRUCTURES
   ============================================================================ */

/**
 * Course structure to store course information
 */
typedef struct {
    int course_id;
    char course_code[MAX_COURSE_CODE];
    char course_name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION];
    int credits;
    int max_capacity;
    int current_enrollment;
    float difficulty_level;
    time_t created_date;
} Course;

/**
 * Student structure to store student information
 */
typedef struct {
    int student_id;
    char name[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
    char phone[20];
    char address[MAX_DESCRIPTION];
    int admission_year;
    char major[MAX_NAME_LENGTH];
    time_t registration_date;
    int is_active;
} Student;

/**
 * Enrollment structure to track student-course relationships
 */
typedef struct {
    int enrollment_id;
    int student_id;
    int course_id;
    float grade;
    char letter_grade;
    float credit_points;
    time_t enrollment_date;
    int status; /* 0: pending, 1: active, 2: completed, 3: dropped */
} Enrollment;

/**
 * Grade record for tracking individual assessments
 */
typedef struct {
    int assessment_id;
    int enrollment_id;
    char assessment_type[50]; /* Quiz, Midterm, Final, Assignment */
    float marks_obtained;
    float total_marks;
    float percentage;
    time_t assessment_date;
} GradeRecord;

/**
 * System statistics and analytics
 */
typedef struct {
    int total_students;
    int total_courses;
    int total_enrollments;
    float average_gpa;
    float highest_gpa;
    float lowest_gpa;
    int courses_offered;
    float average_enrollment_rate;
} SystemStats;

/**
 * Log entry for system operations
 */
typedef struct {
    int log_id;
    int log_level;
    time_t timestamp;
    char operation[MAX_NAME_LENGTH];
    char details[MAX_DESCRIPTION];
} LogEntry;

/* ============================================================================
   GLOBAL VARIABLES
   ============================================================================ */

Student students[MAX_STUDENTS];
Course courses[MAX_COURSES];
Enrollment enrollments[MAX_ENROLLMENTS];
GradeRecord grade_records[MAX_GRADES];
LogEntry system_log[MAX_LOG_ENTRIES];

int student_count = 0;
int course_count = 0;
int enrollment_count = 0;
int grade_record_count = 0;
int log_entry_count = 0;

/* ============================================================================
   UTILITY FUNCTIONS
   ============================================================================ */

/**
 * Log system operations
 */
void log_operation(int level, const char *operation, const char *details) {
    if (log_entry_count >= MAX_LOG_ENTRIES) {
        printf("Warning: Log buffer full\n");
        return;
    }
    
    system_log[log_entry_count].log_id = log_entry_count + 1;
    system_log[log_entry_count].log_level = level;
    system_log[log_entry_count].timestamp = time(NULL);
    strcpy(system_log[log_entry_count].operation, operation);
    strcpy(system_log[log_entry_count].details, details);
    
    log_entry_count++;
}

/**
 * Print separator line for formatting
 */
void print_separator(char character, int length) {
    for (int i = 0; i < length; i++) {
        printf("%c", character);
    }
    printf("\n");
}

/**
 * Clear input buffer
 */
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Validate email format
 */
int is_valid_email(const char *email) {
    int has_at = 0, has_dot = 0;
    
    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') has_at++;
        if (email[i] == '.') has_dot++;
    }
    
    return (has_at == 1 && has_dot >= 1);
}

/**
 * Validate phone number
 */
int is_valid_phone(const char *phone) {
    if (strlen(phone) < 10) return 0;
    
    for (int i = 0; i < strlen(phone); i++) {
        if (!isdigit(phone[i]) && phone[i] != '-' && phone[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

/**
 * Convert numeric grade to letter grade
 */
char get_letter_grade(float marks) {
    if (marks >= GRADE_A_MIN) return 'A';
    if (marks >= GRADE_B_MIN) return 'B';
    if (marks >= GRADE_C_MIN) return 'C';
    if (marks >= GRADE_D_MIN) return 'D';
    return 'F';
}

/**
 * Calculate GPA from letter grade
 */
float get_gpa_from_grade(char grade) {
    switch (grade) {
        case 'A': return 4.0f;
        case 'B': return 3.0f;
        case 'C': return 2.0f;
        case 'D': return 1.0f;
        case 'F': return 0.0f;
        default: return 0.0f;
    }
}

/**
 * Get current date and time as string
 */
void get_current_datetime_string(char *buffer, int size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

/* ============================================================================
   STUDENT MANAGEMENT FUNCTIONS
   ============================================================================ */

/**
 * Add a new student to the system
 */
int add_student(void) {
    if (student_count >= MAX_STUDENTS) {
        printf("Error: Maximum student limit reached!\n");
        log_operation(LOG_ERROR, "Add Student", "Maximum student limit exceeded");
        return 0;
    }
    
    printf("\n");
    print_separator('=', 60);
    printf("                    ADD NEW STUDENT\n");
    print_separator('=', 60);
    
    students[student_count].student_id = student_count + 1001;
    
    printf("Enter student name: ");
    fgets(students[student_count].name, MAX_NAME_LENGTH, stdin);
    students[student_count].name[strcspn(students[student_count].name, "\n")] = 0;
    
    printf("Enter email address: ");
    fgets(students[student_count].email, MAX_NAME_LENGTH, stdin);
    students[student_count].email[strcspn(students[student_count].email, "\n")] = 0;
    
    if (!is_valid_email(students[student_count].email)) {
        printf("Warning: Email format may be invalid\n");
        log_operation(LOG_WARNING, "Add Student", "Invalid email format");
    }
    
    printf("Enter phone number: ");
    fgets(students[student_count].phone, 20, stdin);
    students[student_count].phone[strcspn(students[student_count].phone, "\n")] = 0;
    
    if (!is_valid_phone(students[student_count].phone)) {
        printf("Warning: Phone number format may be invalid\n");
        log_operation(LOG_WARNING, "Add Student", "Invalid phone format");
    }
    
    printf("Enter address: ");
    fgets(students[student_count].address, MAX_DESCRIPTION, stdin);
    students[student_count].address[strcspn(students[student_count].address, "\n")] = 0;
    
    printf("Enter admission year: ");
    scanf("%d", &students[student_count].admission_year);
    clear_input_buffer();
    
    printf("Enter major: ");
    fgets(students[student_count].major, MAX_NAME_LENGTH, stdin);
    students[student_count].major[strcspn(students[student_count].major, "\n")] = 0;
    
    students[student_count].registration_date = time(NULL);
    students[student_count].is_active = 1;
    
    printf("\n✓ Student added successfully with ID: %d\n", students[student_count].student_id);
    
    char log_details[200];
    sprintf(log_details, "Added student: %s (ID: %d)", students[student_count].name, 
            students[student_count].student_id);
    log_operation(LOG_SUCCESS, "Add Student", log_details);
    
    student_count++;
    return 1;
}

/**
 * Display all students
 */
void display_all_students(void) {
    if (student_count == 0) {
        printf("No students in the system.\n");
        return;
    }
    
    printf("\n");
    print_separator('=', 100);
    printf("%-6s %-25s %-30s %-15s %-10s\n", "ID", "Name", "Email", "Phone", "Major");
    print_separator('=', 100);
    
    for (int i = 0; i < student_count; i++) {
        if (students[i].is_active) {
            printf("%-6d %-25s %-30s %-15s %-10s\n",
                   students[i].student_id,
                   students[i].name,
                   students[i].email,
                   students[i].phone,
                   students[i].major);
        }
    }
    
    print_separator('=', 100);
    printf("Total Active Students: %d\n\n", student_count);
}

/**
 * Display student details
 */
void display_student_details(int student_id) {
    for (int i = 0; i < student_count; i++) {
        if (students[i].student_id == student_id && students[i].is_active) {
            printf("\n");
            print_separator('=', 60);
            printf("                    STUDENT DETAILS\n");
            print_separator('=', 60);
            printf("Student ID:      %d\n", students[i].student_id);
            printf("Name:            %s\n", students[i].name);
            printf("Email:           %s\n", students[i].email);
            printf("Phone:           %s\n", students[i].phone);
            printf("Address:         %s\n", students[i].address);
            printf("Admission Year:  %d\n", students[i].admission_year);
            printf("Major:           %s\n", students[i].major);
            printf("Status:          %s\n", students[i].is_active ? "Active" : "Inactive");
            
            char datetime[50];
            get_current_datetime_string(datetime, sizeof(datetime));
            printf("Registration:    %s\n", datetime);
            print_separator('=', 60);
            printf("\n");
            return;
        }
    }
    
    printf("Student not found.\n");
    log_operation(LOG_WARNING, "Display Student", "Student ID not found");
}

/**
 * Search student by name
 */
void search_student_by_name(void) {
    char search_name[MAX_NAME_LENGTH];
    
    printf("Enter student name to search: ");
    fgets(search_name, MAX_NAME_LENGTH, stdin);
    search_name[strcspn(search_name, "\n")] = 0;
    
    printf("\n");
    print_separator('=', 100);
    printf("%-6s %-25s %-30s %-15s %-10s\n", "ID", "Name", "Email", "Phone", "Major");
    print_separator('=', 100);
    
    int found = 0;
    for (int i = 0; i < student_count; i++) {
        if (students[i].is_active && strstr(students[i].name, search_name)) {
            printf("%-6d %-25s %-30s %-15s %-10s\n",
                   students[i].student_id,
                   students[i].name,
                   students[i].email,
                   students[i].phone,
                   students[i].major);
            found++;
        }
    }
    
    print_separator('=', 100);
    
    if (found == 0) {
        printf("No students found matching '%s'\n", search_name);
    } else {
        printf("Found %d student(s)\n", found);
    }
    printf("\n");
}

/* ============================================================================
   COURSE MANAGEMENT FUNCTIONS
   ============================================================================ */

/**
 * Add a new course
 */
int add_course(void) {
    if (course_count >= MAX_COURSES) {
        printf("Error: Maximum course limit reached!\n");
        log_operation(LOG_ERROR, "Add Course", "Maximum course limit exceeded");
        return 0;
    }
    
    printf("\n");
    print_separator('=', 60);
    printf("                     ADD NEW COURSE\n");
    print_separator('=', 60);
    
    courses[course_count].course_id = course_count + 5001;
    
    printf("Enter course code (e.g., CS101): ");
    fgets(courses[course_count].course_code, MAX_COURSE_CODE, stdin);
    courses[course_count].course_code[strcspn(courses[course_count].course_code, "\n")] = 0;
    
    printf("Enter course name: ");
    fgets(courses[course_count].course_name, MAX_NAME_LENGTH, stdin);
    courses[course_count].course_name[strcspn(courses[course_count].course_name, "\n")] = 0;
    
    printf("Enter course description: ");
    fgets(courses[course_count].description, MAX_DESCRIPTION, stdin);
    courses[course_count].description[strcspn(courses[course_count].description, "\n")] = 0;
    
    printf("Enter course credits: ");
    scanf("%d", &courses[course_count].credits);
    
    printf("Enter maximum capacity: ");
    scanf("%d", &courses[course_count].max_capacity);
    
    printf("Enter difficulty level (1.0 - 5.0): ");
    scanf("%f", &courses[course_count].difficulty_level);
    
    clear_input_buffer();
    
    courses[course_count].current_enrollment = 0;
    courses[course_count].created_date = time(NULL);
    
    printf("\n✓ Course added successfully with ID: %d\n", courses[course_count].course_id);
    
    char log_details[200];
    sprintf(log_details, "Added course: %s (%s)", courses[course_count].course_name,
            courses[course_count].course_code);
    log_operation(LOG_SUCCESS, "Add Course", log_details);
    
    course_count++;
    return 1;
}

/**
 * Display all courses
 */
void display_all_courses(void) {
    if (course_count == 0) {
        printf("No courses in the system.\n");
        return;
    }
    
    printf("\n");
    print_separator('=', 120);
    printf("%-6s %-10s %-25s %-8s %-12s %-10s %-15s\n", 
           "ID", "Code", "Name", "Credits", "Capacity", "Enrolled", "Difficulty");
    print_separator('=', 120);
    
    for (int i = 0; i < course_count; i++) {
        printf("%-6d %-10s %-25s %-8d %-12d %-10d %-15.1f\n",
               courses[i].course_id,
               courses[i].course_code,
               courses[i].course_name,
               courses[i].credits,
               courses[i].max_capacity,
               courses[i].current_enrollment,
               courses[i].difficulty_level);
    }
    
    print_separator('=', 120);
    printf("Total Courses: %d\n\n", course_count);
}

/**
 * Display course details
 */
void display_course_details(int course_id) {
    for (int i = 0; i < course_count; i++) {
        if (courses[i].course_id == course_id) {
            printf("\n");
            print_separator('=', 70);
            printf("                      COURSE DETAILS\n");
            print_separator('=', 70);
            printf("Course ID:           %d\n", courses[i].course_id);
            printf("Course Code:         %s\n", courses[i].course_code);
            printf("Course Name:         %s\n", courses[i].course_name);
            printf("Description:         %s\n", courses[i].description);
            printf("Credits:             %d\n", courses[i].credits);
            printf("Maximum Capacity:    %d\n", courses[i].max_capacity);
            printf("Current Enrollment:  %d\n", courses[i].current_enrollment);
            printf("Enrollment Rate:     %.1f%%\n", 
                   (float)courses[i].current_enrollment / courses[i].max_capacity * 100);
            printf("Difficulty Level:    %.1f/5.0\n", courses[i].difficulty_level);
            printf("Available Seats:     %d\n", courses[i].max_capacity - courses[i].current_enrollment);
            print_separator('=', 70);
            printf("\n");
            return;
        }
    }
    
    printf("Course not found.\n");
    log_operation(LOG_WARNING, "Display Course", "Course ID not found");
}

/* ============================================================================
   ENROLLMENT MANAGEMENT FUNCTIONS
   ============================================================================ */

/**
 * Enroll a student in a course
 */
int enroll_student_in_course(void) {
    if (enrollment_count >= MAX_ENROLLMENTS) {
        printf("Error: Maximum enrollment limit reached!\n");
        log_operation(LOG_ERROR, "Enrollment", "Maximum enrollment limit exceeded");
        return 0;
    }
    
    printf("\n");
    print_separator('=', 60);
    printf("                  ENROLL STUDENT\n");
    print_separator('=', 60);
    
    printf("Enter student ID: ");
    int student_id;
    scanf("%d", &student_id);
    
    printf("Enter course ID: ");
    int course_id;
    scanf("%d", &course_id);
    
    clear_input_buffer();
    
    /* Validate student exists */
    int student_index = -1;
    for (int i = 0; i < student_count; i++) {
        if (students[i].student_id == student_id && students[i].is_active) {
            student_index = i;
            break;
        }
    }
    
    if (student_index == -1) {
        printf("Error: Student not found!\n");
        log_operation(LOG_ERROR, "Enrollment", "Student not found");
        return 0;
    }
    
    /* Validate course exists */
    int course_index = -1;
    for (int i = 0; i < course_count; i++) {
        if (courses[i].course_id == course_id) {
            course_index = i;
            break;
        }
    }
    
    if (course_index == -1) {
        printf("Error: Course not found!\n");
        log_operation(LOG_ERROR, "Enrollment", "Course not found");
        return 0;
    }
    
    /* Check capacity */
    if (courses[course_index].current_enrollment >= courses[course_index].max_capacity) {
        printf("Error: Course is at maximum capacity!\n");
        log_operation(LOG_ERROR, "Enrollment", "Course at maximum capacity");
        return 0;
    }
    
    /* Check for duplicate enrollment */
    for (int i = 0; i < enrollment_count; i++) {
        if (enrollments[i].student_id == student_id && 
            enrollments[i].course_id == course_id && 
            enrollments[i].status != 3) {
            printf("Error: Student is already enrolled in this course!\n");
            log_operation(LOG_WARNING, "Enrollment", "Duplicate enrollment attempt");
            return 0;
        }
    }
    
    enrollments[enrollment_count].enrollment_id = enrollment_count + 7001;
    enrollments[enrollment_count].student_id = student_id;
    enrollments[enrollment_count].course_id = course_id;
    enrollments[enrollment_count].grade = 0.0f;
    enrollments[enrollment_count].letter_grade = '-';
    enrollments[enrollment_count].credit_points = 0.0f;
    enrollments[enrollment_count].enrollment_date = time(NULL);
    enrollments[enrollment_count].status = 0; /* pending */
    
    courses[course_index].current_enrollment++;
    
    printf("\n✓ Student successfully enrolled in course!\n");
    printf("  Enrollment ID: %d\n", enrollments[enrollment_count].enrollment_id);
    
    char log_details[200];
    sprintf(log_details, "Enrolled student %d in course %d", student_id, course_id);
    log_operation(LOG_SUCCESS, "Enrollment", log_details);
    
    enrollment_count++;
    return 1;
}

/**
 * View student enrollments
 */
void view_student_enrollments(void) {
    printf("Enter student ID: ");
    int student_id;
    scanf("%d", &student_id);
    clear_input_buffer();
    
    /* Verify student exists */
    int found_student = 0;
    for (int i = 0; i < student_count; i++) {
        if (students[i].student_id == student_id) {
            found_student = 1;
            break;
        }
    }
    
    if (!found_student) {
        printf("Student not found.\n");
        return;
    }
    
    printf("\n");
    print_separator('=', 100);
    printf("%-6s %-25s %-10s %-10s %-8s %-15s\n", 
           "Enr.ID", "Course Name", "Course Code", "Credits", "Grade", "Status");
    print_separator('=', 100);
    
    int enrolled = 0;
    for (int i = 0; i < enrollment_count; i++) {
        if (enrollments[i].student_id == student_id) {
            /* Find course name */
            char course_name[MAX_NAME_LENGTH] = "Unknown";
            char course_code[MAX_COURSE_CODE] = "Unknown";
            int credits = 0;
            
            for (int j = 0; j < course_count; j++) {
                if (courses[j].course_id == enrollments[i].course_id) {
                    strcpy(course_name, courses[j].course_name);
                    strcpy(course_code, courses[j].course_code);
                    credits = courses[j].credits;
                    break;
                }
            }
            
            char status[20] = "Pending";
            if (enrollments[i].status == 1) strcpy(status, "Active");
            else if (enrollments[i].status == 2) strcpy(status, "Completed");
            else if (enrollments[i].status == 3) strcpy(status, "Dropped");
            
            printf("%-6d %-25s %-10s %-10d %-8.1f %-15s\n",
                   enrollments[i].enrollment_id,
                   course_name,
                   course_code,
                   credits,
                   enrollments[i].grade,
                   status);
            enrolled++;
        }
    }
    
    print_separator('=', 100);
    
    if (enrolled == 0) {
        printf("Student has no enrollments.\n");
    } else {
        printf("Total Enrollments: %d\n", enrolled);
    }
    printf("\n");
}

/* ============================================================================
   GRADE MANAGEMENT FUNCTIONS
   ============================================================================ */

/**
 * Record a grade for a student
 */
int record_grade(void) {
    printf("\n");
    print_separator('=', 60);
    printf("                   RECORD GRADE\n");
    print_separator('=', 60);
    
    printf("Enter enrollment ID: ");
    int enrollment_id;
    scanf("%d", &enrollment_id);
    
    printf("Enter grade (0-100): ");
    float grade;
    scanf("%f", &grade);
    
    clear_input_buffer();
    
    if (grade < MIN_GRADE || grade > MAX_GRADE) {
        printf("Error: Grade must be between 0 and 100!\n");
        log_operation(LOG_ERROR, "Record Grade", "Invalid grade value");
        return 0;
    }
    
    /* Find enrollment */
    int enrollment_index = -1;
    for (int i = 0; i < enrollment_count; i++) {
        if (enrollments[i].enrollment_id == enrollment_id) {
            enrollment_index = i;
            break;
        }
    }
    
    if (enrollment_index == -1) {
        printf("Error: Enrollment not found!\n");
        log_operation(LOG_ERROR, "Record Grade", "Enrollment not found");
        return 0;
    }
    
    enrollments[enrollment_index].grade = grade;
    enrollments[enrollment_index].letter_grade = get_letter_grade(grade);
    enrollments[enrollment_index].credit_points = get_gpa_from_grade(enrollments[enrollment_index].letter_grade);
    enrollments[enrollment_index].status = 2; /* completed */
    
    printf("\n✓ Grade recorded successfully!\n");
    printf("  Enrollment ID: %d\n", enrollment_id);
    printf("  Grade: %.2f (%c)\n", grade, enrollments[enrollment_index].letter_grade);
    printf("  GPA Points: %.2f\n", enrollments[enrollment_index].credit_points);
    
    char log_details[200];
    sprintf(log_details, "Recorded grade %.2f for enrollment %d", grade, enrollment_id);
    log_operation(LOG_SUCCESS, "Record Grade", log_details);
    
    return 1;
}

/**
 * Calculate student GPA
 */
void calculate_student_gpa(void) {
    printf("Enter student ID: ");
    int student_id;
    scanf("%d", &student_id);
    clear_input_buffer();
    
    /* Verify student exists */
    int student_index = -1;
    for (int i = 0; i < student_count; i++) {
        if (students[i].student_id == student_id) {
            student_index = i;
            break;
        }
    }
    
    if (student_index == -1) {
        printf("Student not found.\n");
        return;
    }
    
    float total_gpa = 0.0f;
    int completed_courses = 0;
    
    for (int i = 0; i < enrollment_count; i++) {
        if (enrollments[i].student_id == student_id && enrollments[i].status == 2) {
            total_gpa += enrollments[i].credit_points;
            completed_courses++;
        }
    }
    
    printf("\n");
    print_separator('=', 60);
    printf("                    STUDENT GPA\n");
    print_separator('=', 60);
    printf("Student: %s\n", students[student_index].name);
    printf("Student ID: %d\n", student_id);
    printf("Completed Courses: %d\n", completed_courses);
    
    if (completed_courses > 0) {
        float gpa = total_gpa / completed_courses;
        printf("GPA: %.2f\n", gpa);
    } else {
        printf("GPA: N/A (No completed courses)\n");
    }
    
    print_separator('=', 60);
    printf("\n");
}

/* ============================================================================
   ANALYTICS AND REPORTING FUNCTIONS
   ============================================================================ */

/**
 * Display system statistics
 */
void display_system_statistics(void) {
    printf("\n");
    print_separator('=', 80);
    printf("                      SYSTEM STATISTICS\n");
    print_separator('=', 80);
    
    printf("Total Students (Active):    %d\n", student_count);
    printf("Total Courses:              %d\n", course_count);
    printf("Total Enrollments:          %d\n", enrollment_count);
    printf("Total Log Entries:          %d\n", log_entry_count);
    
    /* Calculate average GPA */
    float total_gpa = 0.0f;
    int completed_enrollments = 0;
    
    for (int i = 0; i < enrollment_count; i++) {
        if (enrollments[i].status == 2) {
            total_gpa += enrollments[i].credit_points;
            completed_enrollments++;
        }
    }
    
    if (completed_enrollments > 0) {
        printf("Average GPA (System):       %.2f\n", total_gpa / completed_enrollments);
    }
    
    /* Calculate average enrollment rate */
    float total_enrollment_rate = 0.0f;
    for (int i = 0; i < course_count; i++) {
        if (courses[i].max_capacity > 0) {
            total_enrollment_rate += (float)courses[i].current_enrollment / courses[i].max_capacity;
        }
    }
    
    if (course_count > 0) {
        printf("Average Enrollment Rate:    %.1f%%\n", (total_enrollment_rate / course_count) * 100);
    }
    
    print_separator('=', 80);
    printf("\n");
}

/**
 * Generate class statistics
 */
void generate_class_statistics(void) {
    printf("Enter course ID: ");
    int course_id;
    scanf("%d", &course_id);
    clear_input_buffer();
    
    /* Find course */
    int course_index = -1;
    for (int i = 0; i < course_count; i++) {
        if (courses[i].course_id == course_id) {
            course_index = i;
            break;
        }
    }
    
    if (course_index == -1) {
        printf("Course not found.\n");
        return;
    }
    
    float total_grade = 0.0f;
    float highest_grade = 0.0f;
    float lowest_grade = 100.0f;
    int students_graded = 0;
    
    for (int i = 0; i < enrollment_count; i++) {
        if (enrollments[i].course_id == course_id && enrollments[i].status == 2) {
            total_grade += enrollments[i].grade;
            students_graded++;
            
            if (enrollments[i].grade > highest_grade) {
                highest_grade = enrollments[i].grade;
            }
            if (enrollments[i].grade < lowest_grade) {
                lowest_grade = enrollments[i].grade;
            }
        }
    }
    
    printf("\n");
    print_separator('=', 70);
    printf("                    CLASS STATISTICS\n");
    print_separator('=', 70);
    printf("Course: %s (%s)\n", courses[course_index].course_name, 
           courses[course_index].course_code);
    printf("Course ID: %d\n", course_id);
    printf("Total Enrollment: %d\n", courses[course_index].current_enrollment);
    printf("Students Graded: %d\n", students_graded);
    
    if (students_graded > 0) {
        float average_grade = total_grade / students_graded;
        printf("Average Grade: %.2f\n", average_grade);
        printf("Highest Grade: %.2f\n", highest_grade);
        printf("Lowest Grade: %.2f\n", lowest_grade);
        printf("Grade Range: %.2f\n", highest_grade - lowest_grade);
    } else {
        printf("No grades recorded for this course.\n");
    }
    
    print_separator('=', 70);
    printf("\n");
}

/* ============================================================================
   LOG AND REPORTING FUNCTIONS
   ============================================================================ */

/**
 * Display system log
 */
void display_system_log(void) {
    if (log_entry_count == 0) {
        printf("No log entries.\n");
        return;
    }
    
    printf("\n");
    print_separator('=', 120);
    printf("%-6s %-12s %-20s %-20s %-50s\n", 
           "ID", "Level", "Timestamp", "Operation", "Details");
    print_separator('=', 120);
    
    for (int i = 0; i < log_entry_count; i++) {
        char level_str[20];
        switch (system_log[i].log_level) {
            case LOG_INFO: strcpy(level_str, "INFO"); break;
            case LOG_WARNING: strcpy(level_str, "WARNING"); break;
            case LOG_ERROR: strcpy(level_str, "ERROR"); break;
            case LOG_SUCCESS: strcpy(level_str, "SUCCESS"); break;
            default: strcpy(level_str, "UNKNOWN"); break;
        }
        
        char timestamp[50];
        struct tm *tm_info = localtime(&system_log[i].timestamp);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("%-6d %-12s %-20s %-20s %-50s\n",
               system_log[i].log_id,
               level_str,
               timestamp,
               system_log[i].operation,
               system_log[i].details);
    }
    
    print_separator('=', 120);
    printf("Total Log Entries: %d\n\n", log_entry_count);
}

/**
 * Export data to file
 */
void export_data_to_file(void) {
    FILE *file = fopen("system_export.txt", "w");
    if (!file) {
        printf("Error: Could not create export file!\n");
        log_operation(LOG_ERROR, "Export Data", "Failed to create file");
        return;
    }
    
    fprintf(file, "================== SYSTEM DATA EXPORT ==================\n");
    fprintf(file, "Export Date: ");
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    fprintf(file, "%s\n\n", asctime(tm_info));
    
    /* Export students */
    fprintf(file, "\n============ STUDENTS ============\n");
    fprintf(file, "Total Students: %d\n\n", student_count);
    for (int i = 0; i < student_count; i++) {
        fprintf(file, "ID: %d | Name: %s | Email: %s | Phone: %s | Major: %s\n",
                students[i].student_id,
                students[i].name,
                students[i].email,
                students[i].phone,
                students[i].major);
    }
    
    /* Export courses */
    fprintf(file, "\n============ COURSES ============\n");
    fprintf(file, "Total Courses: %d\n\n", course_count);
    for (int i = 0; i < course_count; i++) {
        fprintf(file, "ID: %d | Code: %s | Name: %s | Credits: %d | Enrolled: %d/%d\n",
                courses[i].course_id,
                courses[i].course_code,
                courses[i].course_name,
                courses[i].credits,
                courses[i].current_enrollment,
                courses[i].max_capacity);
    }
    
    /* Export enrollments */
    fprintf(file, "\n============ ENROLLMENTS ============\n");
    fprintf(file, "Total Enrollments: %d\n\n", enrollment_count);
    for (int i = 0; i < enrollment_count; i++) {
        fprintf(file, "Enrollment ID: %d | Student: %d | Course: %d | Grade: %.2f | Status: %d\n",
                enrollments[i].enrollment_id,
                enrollments[i].student_id,
                enrollments[i].course_id,
                enrollments[i].grade,
                enrollments[i].status);
    }
    
    fprintf(file, "\n========== END OF EXPORT ==========\n");
    
    fclose(file);
    printf("✓ Data exported successfully to 'system_export.txt'\n");
    log_operation(LOG_SUCCESS, "Export Data", "Data exported to file");
}

/* ============================================================================
   MAIN MENU AND INTERFACE
   ============================================================================ */

/**
 * Display main menu
 */
void display_main_menu(void) {
    printf("\n");
    print_separator('=', 70);
    printf("       STUDENT MANAGEMENT AND ANALYTICS SYSTEM v2.0\n");
    print_separator('=', 70);
    printf("\n");
    printf("========== MAIN MENU ==========\n");
    printf("1.  Add Student\n");
    printf("2.  Display All Students\n");
    printf("3.  Search Student by Name\n");
    printf("4.  View Student Details\n");
    printf("5.  Add Course\n");
    printf("6.  Display All Courses\n");
    printf("7.  View Course Details\n");
    printf("8.  Enroll Student in Course\n");
    printf("9.  View Student Enrollments\n");
    printf("10. Record Grade\n");
    printf("11. Calculate Student GPA\n");
    printf("12. Display System Statistics\n");
    printf("13. Generate Class Statistics\n");
    printf("14. Display System Log\n");
    printf("15. Export Data to File\n");
    printf("16. Exit System\n");
    printf("===============================\n");
    printf("Enter your choice (1-16): ");
}

/**
 * Main program function
 */
int main(void) {
    int choice;
    int input_id;
    
    printf("\n");
    printf("**** INITIALIZING STUDENT MANAGEMENT SYSTEM ****\n");
    log_operation(LOG_INFO, "System Init", "System started successfully");
    printf("**** READY ****\n");
    
    while (1) {
        display_main_menu();
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Invalid input. Please enter a number.\n");
            log_operation(LOG_WARNING, "Menu", "Invalid input received");
            continue;
        }
        
        clear_input_buffer();
        
        switch (choice) {
            case 1:
                add_student();
                break;
            case 2:
                display_all_students();
                break;
            case 3:
                search_student_by_name();
                break;
            case 4:
                printf("Enter student ID: ");
                scanf("%d", &input_id);
                clear_input_buffer();
                display_student_details(input_id);
                break;
            case 5:
                add_course();
                break;
            case 6:
                display_all_courses();
                break;
            case 7:
                printf("Enter course ID: ");
                scanf("%d", &input_id);
                clear_input_buffer();
                display_course_details(input_id);
                break;
            case 8:
                enroll_student_in_course();
                break;
            case 9:
                view_student_enrollments();
                break;
            case 10:
                record_grade();
                break;
            case 11:
                calculate_student_gpa();
                break;
            case 12:
                display_system_statistics();
                break;
            case 13:
                generate_class_statistics();
                break;
            case 14:
                display_system_log();
                break;
            case 15:
                export_data_to_file();
                break;
            case 16:
                printf("\n");
                print_separator('=', 70);
                printf("Thank you for using Student Management System!\n");
                printf("System shutting down...\n");
                log_operation(LOG_INFO, "System Shutdown", "System exited normally");
                print_separator('=', 70);
                printf("\n");
                return EXIT_SUCCESS;
            default:
                printf("Invalid choice! Please select a valid option (1-16).\n");
                log_operation(LOG_WARNING, "Menu", "Invalid choice selected");
        }
    }
    
    return EXIT_SUCCESS;
}
