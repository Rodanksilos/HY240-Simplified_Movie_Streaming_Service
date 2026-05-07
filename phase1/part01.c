#include <stdio.h>
#include <stdlib.h>

#include "streaming_service.h"

struct user* user_list; //Head of user list
struct user user_list_guard; //Guard of the user list

struct new_movie* new_movie_list; //Head of the new movie list
struct new_movie new_movie_guard; //Guard of the new movie list

struct movie* movie_list[6]; //Array of pointers to categorized movie list

/*
 * Register User - Event R
 *
 * Adds user with ID uid to
 * users list, as long as nos
 * other user with the same uid
 * already exists.
 *
 * Returns 0 on success, -1 on
 * failure (user ID already exists,
 * malloc or other error)
 */
int register_user(int uid){
    struct user* new_user;
    struct user* node_location = user_list;
    struct user* prev_location;

    while((node_location->uid != -1)&&(node_location->uid < uid)){ //Search through the already registered users
        prev_location = node_location;
        node_location = node_location->next;

    }
    if(node_location->uid == uid){ return -1;}//If a user with this uid already exists

    new_user = malloc(sizeof(struct user)); //If there isn't a user with this uid already, then allocate space for a new user

    if(node_location == user_list){ //If the list is empty
        new_user->next = user_list;
        user_list = new_user;
    }
    else{ //If the list has other users
        new_user->next = node_location;
        prev_location->next = new_user;
    }

    new_user->uid = uid; // Assign values to the new user
    new_user->watchHistory = NULL;
    new_user->suggestedHead = NULL;
    new_user->suggestedTail = NULL;

    node_location = user_list; //print all the users
    printf("R %d\n", uid);
    printf("    Users = ");
    while(node_location->uid != -1){
        printf("%d ",node_location->uid);
        node_location = node_location->next;
    }
    printf("\nDONE\n");

    return 0;

}

/*
 * Unregister User - Event U
 *
 * Removes user with ID uid from
 * users list, as long as such a
 * user exists, after clearing the
 * user's suggested movie list and
 * watch history stack
 */
void unregister_user(int uid){
    struct user* node_location = user_list;
    struct user* prev_location = NULL;
    while((node_location->uid != -1) && (node_location->uid!=uid)){ //Search for the given uid in users list
        prev_location = node_location;
        node_location = node_location->next;
    }
    if(node_location->uid == -1){ return;} //There doesn't exist a user with the given uid

    //If the user exists then delete their watch history and suggested movies list
    void* tmp;
    struct movie* moviep;
    struct suggested_movie* suggested_moviep;

    moviep = node_location->watchHistory; //Go through and delete their watch history
    while(moviep != NULL){
        tmp = moviep->next;
        free(moviep);
        moviep = tmp;
    }

    suggested_moviep = node_location->suggestedHead; //Go through and delete their suggested movies
    while(suggested_moviep != NULL){
        tmp = suggested_moviep->next;
        free(suggested_moviep);
        suggested_moviep = tmp;
    }

    if(node_location == user_list){ //If the user is the first on the list
        user_list = node_location->next;
    }
    else{ //If the user is elsewhere in the list
        prev_location->next = node_location->next;
    }
    free(node_location);

    node_location = user_list; //print the new users list
    printf("U %d\n", uid);
    printf("    Users = ");
    while(node_location->uid != -1){
        printf("%d ",node_location->uid);
        node_location = node_location->next;
    }
    printf("\nDONE\n");

}

/*
 * Add new movie - Event A
 *
 * Adds movie with ID mid, category
 * category and release year year
 * to new movies list. The new movies
 * list must remain sorted (increasing
 * order based on movie ID) after every
 * insertion.
 *
 * Returns 0 on success, -1 on failure
 */
int add_new_movie(unsigned mid, movieCategory_t category, unsigned year){

    if(category >5) {return -1;}//If the category isn't between 0 and 5 then exit

    struct new_movie* node_location = new_movie_list;
    struct new_movie* prev_location;
    while((node_location->info.mid != -1)&&(node_location->info.mid < mid)){ //Find the correct location for the movie
        prev_location = node_location;
        node_location = node_location->next;

    }
    if(node_location->info.mid == mid){return -1;}//If a movie with this mid already exists

    struct new_movie* new_new_movie = malloc(sizeof(struct new_movie)); //Allocate space for new new_movie object
    new_new_movie->category = category;
    new_new_movie->info.mid = mid; //Assign values to the new movie
    new_new_movie->info.year = year;

    if(node_location == new_movie_list){ //If we need to place it at the front
        new_new_movie->next = new_movie_list;
        new_movie_list = new_new_movie;
    }
    else{ //If we need to place it elsewhere
        new_new_movie->next = node_location;
        prev_location->next = new_new_movie;
    }

    new_new_movie->info.mid = mid; // Assign values to the new new_movie
    new_new_movie->info.year = year;
    new_new_movie->category = category;


    node_location = new_movie_list; //Print all the movies
    printf("A %d %d %d\n", mid, category, year);
    printf("    New movies = ");
    while(node_location->info.mid != -1){
        printf("%d %d %d  ", node_location->info.mid, node_location->category, node_location->info.year);
        node_location = node_location->next;
    }
    printf("\nDONE\n");

    return 0;
}

/*
 * Distribute new movies - Event D
 *
 * Distributes movies from the new movies
 * list to the per-category sorted movie
 * lists of the category list array. The new
 * movies list should be empty after this
 * event. This event must be implemented in
 * O(n) time complexity, where n is the size
 * of the new movies list
 */
void distribute_new_movies(void){
    struct movie* tails[6]={NULL, NULL, NULL, NULL, NULL, NULL}; //Temporary tails for the 6 categories so it can be executed in O(n)
    int category;
    struct movie* tmp_movie;
    struct new_movie* tmp;
    while(new_movie_list->info.mid != -1){ //Go through all the movies in new releases
        category = new_movie_list->category; //category variable holds the category (as an int) of the current movie

        tmp_movie = malloc(sizeof(struct movie));
        tmp_movie->info = new_movie_list->info;

        if(movie_list[category] == NULL){ //If it is the first movie of this category
            movie_list[category] = tmp_movie;
            tails[category] = tmp_movie;
            tails[category]->next = NULL;
            tmp_movie->next = NULL;
        }
        else{ //If it isn't the first
            tails[category]->next = tmp_movie;
            tails[category] = tmp_movie;
            tails[category]->next = NULL;
            tmp_movie->next = NULL;
        }
        tmp = new_movie_list->next;
        free(new_movie_list);
        new_movie_list = tmp;
    }


    printf("D\nCategorized Movies:\n"); //Print all the categories

    printf("    Horror:");
    tmp_movie=movie_list[0];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");


    printf("    Sci-fi:");
    tmp_movie=movie_list[1];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Drama:");
    tmp_movie=movie_list[2];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Romance:");
    tmp_movie=movie_list[3];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Documentary:");
    tmp_movie=movie_list[4];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Comedy:");
    tmp_movie=movie_list[5];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }

    printf("\nDONE\n");


}

/*
 * User watches movie - Event W
 *
 * Adds a new struct movie with information
 * corresponding to those of the movie with ID
 * mid to the top of the watch history stack
 * of user uid.
 *
 * Returns 0 on success, -1 on failure
 * (user/movie does not exist, malloc error)
 */
int watch_movie(int uid, unsigned mid) {
    struct user *user_location = user_list;

    while ((user_location->uid != -1) && (user_location->uid < uid)) { //Search through the user list for the user with the given uid
        user_location = user_location->next;
    }
    if (user_location->uid != uid) { return -1; } //If this uid doesn't exist

    struct movie *movie_location;
    int category=0;

    while(category<6){ //Search through the categories for the movie with the given mid
        movie_location = movie_list[category];
        while ((movie_location != NULL) && (movie_location->info.mid < mid)) { //Search the category until we find a mid>= than the given mid or until the end of the list
            movie_location = movie_location->next;
        }

        if((movie_location != NULL) && (movie_location->info.mid == mid)){ break;} //If we didn't hit end of the list, check if the mid is equal to our given mid
        category++; //If we didn't find the movie in this category
    }
    if ((movie_location == NULL )|| (movie_location->info.mid != mid)) { return -1; } //If movie with given mid don't exist


    struct movie* new_watched = malloc(sizeof(struct movie)); //Allocate space for new movie
    new_watched->info = movie_location->info;
    if(user_location->watchHistory == NULL){ //Add movie to user's watch history stack
        new_watched->next = NULL;
    }
    else{
        new_watched->next = user_location->watchHistory;
    }
    user_location->watchHistory = new_watched;

    printf("W %d %d\n", uid, mid); //Print all of user's watch history
    printf("    User %d Watch History = ", uid);
    struct movie* moviep = user_location->watchHistory;
    while(moviep->next != NULL){
        printf("%d, ", moviep->info.mid);
        moviep = moviep->next;
    }
    printf("%d", moviep->info.mid);
    printf("\nDONE\n");
    return 0;
}

/*
 * Suggest movies to user - Event S
 *
 * For each user in the users list with
 * id != uid, pops a struct movie from the
 * user's watch history stack, and adds a
 * struct suggested_movie to user uid's
 * suggested movies list in alternating
 * fashion, once from user uid's suggestedHead
 * pointer and following next pointers, and
 * once from user uid's suggestedTail pointer
 * and following prev pointers. This event
 * should be implemented with time complexity
 * O(n), where n is the size of the users list
 *
 * Returns 0 on success, -1 on failure
 */
int suggest_movies(int uid){
    struct user *user_location = user_list;

    while ((user_location->uid != -1) && (user_location->uid < uid)) { //Search through the user list for the user with the given uid
        user_location = user_location->next;

    }
    if (user_location->uid != uid) { return -1; } //If this uid doesn't exist


    struct suggested_movie* new_suggested;
    struct suggested_movie* frontend = NULL; //Points to the most recent node placed from the front
    struct suggested_movie* rearend = NULL; //Points to the most recent node placed from the back
    struct movie* tmp;
    int front_or_rear=0; //If 0 then place from the front, if 1 then place from the rear
    struct user* userp = user_list;
    while(userp->uid != -1){ //go through all the user with uid different from the given uid
        if((userp->uid != uid) && (userp->watchHistory != NULL)){
            new_suggested = malloc(sizeof(struct suggested_movie)); //If their watch history isn't empty, Pop one movie from the stack
            new_suggested->info = userp->watchHistory->info;
            tmp = userp->watchHistory->next;
            free(userp->watchHistory); //I don't know why but you asked for this
            userp->watchHistory = tmp;
            if(front_or_rear == 0){ //Place in the front
                if(frontend == NULL){
                    user_location->suggestedHead = new_suggested;
                    new_suggested->prev = NULL;
                    frontend = new_suggested;
                }else{
                    new_suggested->prev = frontend;
                    frontend->next = new_suggested;
                    frontend = new_suggested;
                }
                front_or_rear = 1;
            }
            else{ //Place in the back
                if(rearend == NULL){
                    user_location->suggestedTail = new_suggested;
                    new_suggested->next = NULL;
                    rearend = new_suggested;
                }else{
                    new_suggested->next = rearend;
                    rearend->prev = new_suggested;
                    rearend = new_suggested;
                }
                front_or_rear = 0;
            }
        }
        userp = userp->next;
    }
    //Connect front and rear together
    if(rearend != NULL){ //if we have at least 2 movie suggestions
        frontend->next = rearend;
        rearend->prev = frontend;
    }else if(frontend!= NULL){ //If only one suggestion
        frontend->next = NULL;
        user_location->suggestedTail = frontend;
    }

    printf("S %d\n", uid); //Print user's recommendations
    printf("    User %d Suggested Movies = ", uid);
    struct suggested_movie* suggestTmp = user_location->suggestedHead;
    if(suggestTmp != NULL){
        while(suggestTmp->next!= NULL){
            printf("%d, ",suggestTmp->info.mid);
            suggestTmp = suggestTmp->next;
        }
        printf("%d", suggestTmp->info.mid);
    }
    printf("\nDONE\n");
    return 0;
}

/*
 * Filtered movie search - Event F
 *
 * User uid asks to be suggested movies
 * belonging to either category1 or category2
 * and with release year >= year. The resulting
 * suggested movies list must be sorted with
 * increasing order based on movie ID (as the
 * two category lists). This event should be
 * implemented with time complexity O(n + m),
 * where n, m are the sizes of the two category lists
 *
 * Returns 0 on success, -1 on failure
 */
int filtered_movie_search(int uid, movieCategory_t category1, movieCategory_t category2, unsigned year){
    struct user *user_location = user_list;
    while ((user_location->uid != -1) && (user_location->uid < uid)) { //Search through the user list for the user with the given uid
        user_location = user_location->next;
    }
    if (user_location->uid != uid) { return -1; } //If this uid doesn't exist

    if(category1>5 || category2>5) { return -1; } //If the given categories are invalid/out of bounds


    struct suggested_movie *filtered_list = NULL;
    struct suggested_movie *tail_filtered;
    struct suggested_movie *new_filtered;
    struct movie* moviep1 = movie_list[category1];
    struct movie* moviep2 = movie_list[category2];
    while((moviep1 != NULL) || (moviep2 != NULL)){ //Go through both category lists
        if((moviep1 != NULL) && (moviep2!= NULL)){ //If we haven't finished both
            if(moviep1->info.mid > moviep2->info.mid){
                if(moviep2->info.year >= year){ //Check year of release
                    new_filtered = malloc(sizeof(struct suggested_movie));
                    if(filtered_list == NULL){ //If it is the first movie for the list
                        filtered_list = new_filtered;
                        filtered_list->prev = NULL;
                    }else{
                        tail_filtered->next = new_filtered;
                        new_filtered->prev = tail_filtered;
                    }
                    new_filtered->info = moviep2->info; //Copy info to new suggested movie
                    tail_filtered = new_filtered; //Update the tail
                    new_filtered->next = NULL;
                }
                moviep2 = moviep2->next;
            } else{
                if(moviep1->info.year >= year){ //Check year of release
                    new_filtered = malloc(sizeof(struct suggested_movie));
                    if(filtered_list == NULL){ //If it is the first movie for the list
                        filtered_list = new_filtered;
                        filtered_list->prev = NULL;
                    }else{
                        tail_filtered->next = new_filtered;
                        new_filtered->prev = tail_filtered;
                    }
                    new_filtered->info = moviep1->info; //Copy info to new suggested movie
                    tail_filtered = new_filtered; //Update the tail
                    new_filtered->next = NULL;
                }
                moviep1 = moviep1->next;
            }
        }else if(moviep1 != NULL){ //If we have gone through category2 already
            if(moviep1->info.year >= year){ //Check year of release
                new_filtered = malloc(sizeof(struct suggested_movie));
                if(filtered_list == NULL){ //If it is the first movie for the list
                    filtered_list = new_filtered;
                    filtered_list->prev = NULL;
                }else{
                    tail_filtered->next = new_filtered;
                    new_filtered->prev = tail_filtered;
                }
                new_filtered->info = moviep1->info; //Copy info to new suggested movie
                tail_filtered = new_filtered; //Update the tail
                new_filtered->next = NULL;
            }
            moviep1 = moviep1->next;
        }else{ //If we have gone through category1 already
            if(moviep2->info.year >= year){ //Check year of release
                new_filtered = malloc(sizeof(struct suggested_movie));
                if(filtered_list == NULL){ //If it is the first movie for the list
                    filtered_list = new_filtered;
                    filtered_list->prev = NULL;
                }else{
                    tail_filtered->next = new_filtered;
                    new_filtered->prev = tail_filtered;
                }
                new_filtered->info = moviep2->info; //Copy info to new suggested movie
                tail_filtered = new_filtered; //Update the tail
                new_filtered->next = NULL;
            }
            moviep2 = moviep2->next;
        }
    }
    if(user_location->suggestedHead == NULL){ //If the user doesn't have any recommended movies, make the filtered list their suggestions
        user_location->suggestedHead = filtered_list;
        user_location->suggestedTail = tail_filtered;
    }else{ //If the user has suggested movies, add filtered list at the end
        user_location->suggestedTail->next = filtered_list;
        filtered_list->prev = user_location->suggestedTail;
    }
    printf("F %d %d %d %d\n", uid, category1, category2, year); //Print the user's suggestions
    printf("    User %d Suggested Movies = ", uid);
    struct suggested_movie* suggestTmp = user_location->suggestedHead;
    if(suggestTmp != NULL){
        while(suggestTmp->next!= NULL){
            printf("%d, ",suggestTmp->info.mid);
            suggestTmp = suggestTmp->next;
        }
        printf("%d", suggestTmp->info.mid);
    }
    printf("\nDONE\n");
    return 0;
}

/*
 * Take off movie - Event T
 *
 * Movie mid is taken off the service. It is removed
 * from every user's suggested list -if present- and
 * from the corresponding category list.
 */
void take_off_movie(unsigned mid){
    struct movie *prev_movie;
    struct movie *movie_location;
    int category=0;

    while(category<6){
        movie_location = movie_list[category];
        while ((movie_location != NULL) && (movie_location->info.mid < mid)) { //Search the category until we find a mid>= than the given mid or until the end of the list
            prev_movie = movie_location;
            movie_location = movie_location->next;
        }
        if((movie_location != NULL) && (movie_location->info.mid == mid)){ break;} //If we didn't hit end of the list, check if the mid is equal to our given mid
        category++; //If we didn't find the movie in this category
    }
    if ((movie_location == NULL ) || (movie_location->info.mid != mid)) { exit(-1); } //Movie don't exist

    //Remove from category list
    if(movie_list[category] == movie_location){ //if it is the first movie of the category
        movie_list[category] = movie_location->next;
    }
    else{ //if it is somewhere else in the list

        prev_movie->next = movie_location->next;
    }
    free(movie_location);

    //Remove from user suggestions
    printf("T %d\n", mid);

    struct user* userp = user_list;
    struct suggested_movie* smoviep;
    struct suggested_movie* prev_smovie;
    while(userp->uid != -1){ //Go through all the users
        smoviep = userp->suggestedHead; //Check user's suggested list
        if(smoviep != NULL){
            while((smoviep->next!= NULL) && (smoviep->info.mid != mid)){
                prev_smovie = smoviep;
                smoviep = smoviep->next;
            }
            if((smoviep != NULL) && (smoviep->info.mid == mid)){ //If the user has watched that movie
                if(userp->suggestedHead == smoviep){ //If it is the fist node
                    userp->suggestedHead = smoviep->next;
                }else{ //If it is elsewhere in the list
                    prev_smovie->next = smoviep->next;
                }
                printf("    %d was removed from %d suggested list.\n", smoviep->info.mid, userp->uid);
                free(smoviep);
            }
        }
        userp = userp->next;
    }
    printf("    %d removed from %d category list\n", mid, category);
    printf("    Category list = "); //Print the new category list
    struct movie* moviep = movie_list[category];
    if(moviep!= NULL){
        while(moviep->next != NULL){
            printf("%d, ", moviep->info.mid);
            moviep = moviep->next;
        }
        printf("%d", moviep->info.mid);
    }


    printf("\nDONE\n");
}

/*
 * Print movies - Event M
 *
 * Prints information on movies in
 * per-category lists
 */
void print_movies(void){
    struct movie* tmp_movie;
    printf("M\nCategorized Movies:\n"); //Print all the movie categories

    printf("    Horror:");
    tmp_movie=movie_list[0];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");


    printf("    Sci-fi:");
    tmp_movie=movie_list[1];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Drama:");
    tmp_movie=movie_list[2];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Romance:");
    tmp_movie=movie_list[3];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Documentary:");
    tmp_movie=movie_list[4];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }
    printf("\n");

    printf("    Comedy:");
    tmp_movie=movie_list[5];
    while(tmp_movie!=NULL){
        printf("%d ", tmp_movie->info.mid);
        tmp_movie = tmp_movie->next;
    }

    printf("\nDONE\n");
}

/*
 * Print users - Event P
 *
 * Prints information on users in
 * users list
 */
void print_users(void){
    struct user* node_location;
    struct suggested_movie* suggestTmp;
    struct movie* moviep;
    node_location = user_list;

    printf("P\nUsers:\n");
    while(node_location->uid != -1){ //Go through all the users
        printf("  %d:\n",node_location->uid);

        printf("    Suggested: "); //Print their suggested movies
        suggestTmp = node_location->suggestedHead;
        if(suggestTmp != NULL){
            while(suggestTmp->next!= NULL){
                printf("%d, ",suggestTmp->info.mid);
                suggestTmp = suggestTmp->next;
            }
            printf("%d", suggestTmp->info.mid);
        }

        printf("\n    Watch History: "); //Print their watch history
        moviep = node_location->watchHistory;
        if(moviep != NULL){
            while(moviep->next != NULL){
                printf("%d, ", moviep->info.mid);
                moviep = moviep->next;
            }
            printf("%d", moviep->info.mid);
        }
        printf("\n");

        node_location = node_location->next;
    }
    printf("DONE\n");
}