#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Yorum satırları ile anlaşılabilirlik katmaya çalıştım (TR)
// Tried to make it understandable with comment lines (EN)

typedef struct Task
{
    char title[50];
    int priority;
    char description[100];
    char due_date[11];
    int time_estimate;
    char status[20];
    int progress;
    struct Task *next;
} Task;

typedef struct TaskList
{
    Task *head;
} TaskList;

void save_tasks(TaskList *task_list)
{
    FILE *file = fopen("tasks.txt", "w");
    if (file == NULL)
    {
        // Warn user about the errors
        fprintf(stderr, "Error opening file for writing\n");
        exit(EXIT_FAILURE);
    }

    Task *current = task_list->head;
    while (current != NULL)
    {
        // fscanf("%[^\n]") ile \n'e kadar okuyacak
        // kendimce çözüm ürettim fakat kötü bir kod oldu
        strcat(current->title, "\n");
        strcat(current->description, "\n");
        fprintf(file, "%s %d %s %s %d %s %d\n", current->title, current->priority,
                current->description, current->due_date, current->time_estimate,
                current->status, current->progress);
        current = current->next;
    }

    fclose(file);
}

void load_tasks(TaskList *task_list)
{
    // Open the file for reading
    FILE *file = fopen("tasks.txt", "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file for reading\n");
        exit(EXIT_FAILURE);
        // exit(EXIT_FAILURE); ile programı sonlandırıp hata mesajı veriyor
    }

    Task task;
    //%[^\n]newline'a kadar okuyor (gpt yardımı)
    while (fscanf(file, " %[^\n] %d %[^\n] %s %d %s %d",
                  task.title, &task.priority, task.description,
                  task.due_date, &task.time_estimate, task.status,
                  &task.progress) == 7)
    { // 7 task başarılı şekilde yapılana kadar oku

        Task *new_task = malloc(sizeof(Task));
        if (new_task == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        memcpy(new_task, &task, sizeof(Task));
        new_task->next = task_list->head;
        task_list->head = new_task;
    }

    fclose(file);
}

void add_task_ui(TaskList *task_list)
{
    Task *new_task = malloc(sizeof(Task));
    if (new_task == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    printf("\nEnter task title: ");
    scanf(" %[^\n]", new_task->title);
    strcat(new_task->title, "\n");
    // newline eklememin sebebi, eğer dosyaya eklerken ve okurken bunu kullanmazsam
    // tam olarak okuyamıyordu.
    printf("Enter task priority(1,2 or 3): ");
    scanf("%d", &new_task->priority);
    while (new_task->priority != 1 && new_task->priority != 2 && new_task->priority != 3)
    {
        printf("Please enter(1,2 or 3): ");
        scanf("%d", &new_task->priority);
    }
    getchar(); // to avoid next scanf problems
    printf("Enter task description: ");
    scanf(" %[^\n]", new_task->description);
    strcat(new_task->description, "\n");
    printf("Enter due date (YYYY-MM-DD): ");
    scanf("%s", new_task->due_date);
    printf("Enter time estimate in minutes: ");
    scanf("%d", &new_task->time_estimate);
    printf("Enter task status(notStarted, inProgress, completed): ");
    scanf("%s", new_task->status);
    printf("Enter task progress (0-100): ");
    scanf("%d", &new_task->progress);

    new_task->next = task_list->head;
    task_list->head = new_task;

    save_tasks(task_list);
}

void update_task_status_ui(TaskList *task_list)
{
    char title[50];
    printf("\nEnter the title of the task to update status: ");
    scanf("%s", title);

    Task *current = task_list->head;
    while (current != NULL)
    {
        // strcmp ile string karşılaştırması yaptım
        if (strcmp(current->title, title) == 0)
        {
            printf("Enter the new status: ");
            scanf("%s", current->status);
            break;
        }
        current = current->next;
    }

    save_tasks(task_list);
}

void update_task_progress_ui(TaskList *task_list)
{
    char title[50];
    printf("\nEnter the title of the task to update progress: ");
    scanf("%s", title);

    Task *current = task_list->head;
    while (current != NULL)
    {
        // strcmp ile string karşılaştırması yaptım
        if (strcmp(current->title, title) == 0)
        {
            printf("Enter the new progress percentage: ");
            scanf("%d", &current->progress);
            break;
        }
        current = current->next;
    }

    save_tasks(task_list);
}

void delete_task_ui(TaskList *task_list)
{
    char title[50];
    printf("\nEnter the title of the task to delete: ");
    scanf("%s", title);

    Task *current = task_list->head;
    Task *prev = NULL;

    while (current != NULL)
    {
        // strcmp ile string karşılaştırması yaptım
        if (strcmp(current->title, title) == 0)
        {
            if (prev == NULL)
            {
                task_list->head = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current);
            break;
        }
        prev = current;
        current = current->next;
    }
    // after delete, save the tasks to manipulate .txt file
    save_tasks(task_list);
}

void list_tasks_ui(TaskList *task_list)
{
    Task *current = task_list->head;
    printf("---------------------------\n");
    // basic linked list display method
    while (current != NULL)
    {
        printf("\nTitle: %s\nPriority: %d\nDescription: %s\nDue Date: %s\nStatus: %s\nTime Estimate: %dmin\nProgress: %d%%\n",
               current->title, current->priority, current->description, current->due_date,
               current->status, current->time_estimate, current->progress);
        printf("---------------------------\n");
        current = current->next;
    }
}

void print_tasks_sorted_by_priority(TaskList *task_list)
{
    if (task_list == NULL || task_list->head == NULL)
    {
        // Empty list, nothing to print
        return;
    }
    Task *current = task_list->head;
    Task *temp = NULL;

    while (current != NULL)
    {
        Task *index = current->next;

        while (index != NULL)
        {
            // If the current task's priority is less than the index task's priority, swap them
            if (current->priority < index->priority)
            {
                // Swap data
                temp = malloc(sizeof(Task));
                if (temp == NULL)
                {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(EXIT_FAILURE);
                }
                // gpt helped me to use memcpy, copies the data from the index to temp
                memcpy(temp, current, sizeof(Task) - sizeof(temp->next));
                memcpy(current, index, sizeof(Task) - sizeof(current->next));
                memcpy(index, temp, sizeof(Task) - sizeof(index->next));
                free(temp);
            }

            index = index->next;
        }

        current = current->next;
    }

    // Print the sorted tasks
    current = task_list->head;
    while (current != NULL)
    {
        printf("---------------------------\n");
        printf("Title: %s\nPriority: %d\nDescription: %s\nDue Date: %s\nTime Estimate: %d\nStatus: %s\nProgress: %d%%\n",
               current->title, current->priority, current->description, current->due_date,
               current->time_estimate, current->status, current->progress);

        printf("---------------------------\n");

        current = current->next;
    }
}

void free_tasks(TaskList *task_list)
{
    // This function is used to free the memory allocated for the tasks
    Task *current = task_list->head;
    Task *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    task_list->head = NULL;
}

void print_tasks_sorted_by_status(TaskList *task_list)
{
    if (task_list == NULL || task_list->head == NULL)
    {
        // Empty list, nothing to print
        return;
    }

    // I created three separate linked lists for each status
    TaskList notStartedList, inProgressList, completedList;
    notStartedList.head = inProgressList.head = completedList.head = NULL;

    Task *current = task_list->head;

    while (current != NULL)
    {
        // Create a new task node and copy data
        Task *new_task = malloc(sizeof(Task));
        if (new_task == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        memcpy(new_task, current, sizeof(Task));
        new_task->next = NULL;

        // Add the task to the corresponding status list
        if (strcmp(current->status, "notStarted") == 0)
        {
            new_task->next = notStartedList.head;
            notStartedList.head = new_task;
        }
        else if (strcmp(current->status, "inProgress") == 0)
        {
            new_task->next = inProgressList.head;
            inProgressList.head = new_task;
        }
        else if (strcmp(current->status, "completed") == 0)
        {
            new_task->next = completedList.head;
            completedList.head = new_task;
        }

        current = current->next;
    }

    // Print tasks for each status
    printf("\nTasks with status 'Not Started':\n");
    list_tasks_ui(&notStartedList);

    printf("\nTasks with status 'In Progress':\n");
    list_tasks_ui(&inProgressList);

    printf("\nTasks with status 'Completed':\n");
    list_tasks_ui(&completedList);

    // Free the memory allocated for the temporary lists
    free_tasks(&notStartedList);
    free_tasks(&inProgressList);
    free_tasks(&completedList);
}

// sıralama ve kategorize algoritmalarını yetersiz bilgimden dolayı AI ile implement ettim
void filter_and_sort_tasks_ui(TaskList *task_list)
{
    int choice;
    printf("\n1. Order by priority\n");
    printf("2. Order by status\n");
    printf("Enter here: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        // Sort tasks by priority in ascending order
        print_tasks_sorted_by_priority(task_list);
    }
    else if (choice == 2)
    {
        // Categorize tasks by status and print each category separately
        print_tasks_sorted_by_status(task_list);
    }
    else
    {
        printf("Invalid choice\n");
        return;
    }
    return;
}

void search_task_ui(TaskList *task_list)
{
    char keyword[50];
    printf("\nEnter a keyword to search for: ");
    scanf("%s", keyword);
    // bassic search algorithm
    Task *current = task_list->head;
    while (current != NULL)
    {
        if (strstr(current->title, keyword) != NULL || strstr(current->description, keyword) != NULL)
        {
            printf("\nTitle: %s\nPriority: %d\nDescription: %s\nDue Date: %s\nStatus: %s\nTime Estimate: %d\nProgress: %d\n",
                   current->title, current->priority, current->description, current->due_date,
                   current->status, current->time_estimate, current->progress);
            printf("---------------------------\n");
        }
        current = current->next;
    }
}

// C dilinde tarihler hakkında hiçbir bilgim yoktu
// gpt ve time.h dökümanı ile ortaya ne çıkarsa denedim
void delete_overdue_tasks(TaskList *task_list)
{
    time_t current_time;
    time(&current_time);

    Task *current = task_list->head;
    Task *prev = NULL;

    while (current != NULL)
    {
        struct tm due_date_tm = {0}; // Initialize with zeros

        // Parse the due date using sscanf
        if (sscanf(current->due_date, "%d-%d-%d", &due_date_tm.tm_year, &due_date_tm.tm_mon, &due_date_tm.tm_mday) == 3)
        {
            // Adjust year and month
            due_date_tm.tm_year -= 1900; // Adjust year to be since 1900
            due_date_tm.tm_mon -= 1;     // Adjust month to be in the range 0-11
            due_date_tm.tm_isdst = 0;    // Disable daylight saving time
            due_date_tm.tm_mday += 14;   // added 7 days
            // Convert due date to time_t for comparison
            time_t due_date_time = mktime(&due_date_tm);

            // Print the formatted due date for testing
            // printf("Due Date: %d-%02d-%02d\n", due_date_tm.tm_year + 1900, due_date_tm.tm_mon + 1, due_date_tm.tm_mday);

            // Calculate the difference in days
            double difference = difftime(due_date_time, current_time);
            // Test print
            // printf("Difference in days: %f\n", difference / (24 * 60 * 60));

            // Burası oldukça karmaşık oldu, printf ile test ettiğim verilere göre
            // tarihi gelen değerin 7 günü oluyor, 7 günü geçerse different değeri 0. küsürlü değere geliyor ve siliyorum
            if (difference / (24 * 60 * 60) < 1)
            {
                // Due date has passed, delete the task
                if (prev == NULL)
                {
                    task_list->head = current->next;
                }
                else
                {
                    prev->next = current->next;
                }

                // Free the memory occupied by the current task
                free(current);

                // Update the current pointer to the next task or head
                current = prev == NULL ? task_list->head : prev->next;

                // Continue to the next iteration of the loop
                continue;
            }
        }
        else
        {
            // Invalid date format
            printf("Invalid date format for task: %s\n", current->title);
        }

        prev = current;
        current = current->next;
    }
}

void main()
{

    TaskList task_list;
    task_list.head = NULL;

    // This takes time to check file exist or not
    FILE *file = fopen("tasks.txt", "r");
    if (file == NULL)
    {
        // If the file doesn't exist, create it
        file = fopen("tasks.txt", "w");
        if (file == NULL)
        {
            fprintf(stderr, "Error creating tasks.txt\n");
            exit(EXIT_FAILURE);
        }
        fclose(file);
    }
    else
    {
        // If the file exists, close it before loading tasks
        fclose(file);
        // Load tasks from the file
        load_tasks(&task_list);
    }

    /* EĞER KENDI KENDINE SILIYORSA MUHTEMELEN TARIHTENDIR HOCAM,
    BU FONKSIYONU SILIP GORMEZDEN GELEBILIRSINIZ */
    // Check if passed 1 week for each task
    delete_overdue_tasks(&task_list);

    int choice;
    while (1)
    {
        printf("\n1. Add Task");
        printf("\n2. Update Task Status");
        printf("\n3. Update Task Progress");
        printf("\n4. Delete Task");
        printf("\n5. List Tasks");
        printf("\n6. Filter and Sort Tasks");
        printf("\n7. Search Task");
        printf("\n8. Save and Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            add_task_ui(&task_list);
        }
        else if (choice == 2)
        {
            update_task_status_ui(&task_list);
        }
        else if (choice == 3)
        {
            update_task_progress_ui(&task_list);
        }
        else if (choice == 4)
        {
            delete_task_ui(&task_list);
        }
        else if (choice == 5)
        {
            list_tasks_ui(&task_list);
        }
        else if (choice == 6)
        {
            filter_and_sort_tasks_ui(&task_list);
        }
        else if (choice == 7)
        {
            search_task_ui(&task_list);
        }
        else if (choice == 8)
        {
            save_tasks(&task_list);
            break;
        }
    }
}