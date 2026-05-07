/*****************************************************
 * @file   Movie.c                                   *
 * @author Paterakis Giorgos <geopat@csd.uoc.gr>     *
 *                                                   *
 * @brief Implementation for Movie.h 				 *
 * Project: Winter 2023						         *
 *****************************************************/
#include "Movie.h"

//My additional functions

 /**
 * @brief Takes a leaf and find the next one
 *
 * @param node The leaf of which we want to find the next of
 * @return Address of the next leaf
 *         NULL if the isn't another leaf
 */
 userMovie_t* FindNextLeaf(userMovie_t* node){
    if(node->parent == NULL) return NULL;

    userMovie_t* branch = NULL;

    if(node->parent->lc == node){ //The next leaf of a left child leaf is always its right sibling
        branch = node->parent->rc;
        while(branch->lc != NULL) branch=branch->lc;
    }
    else{
        branch = node->parent;
        while (branch->movieID < node->movieID){
            branch=branch->parent;
            if(branch == NULL) return NULL; //node is the last leaf with the biggest mid
        }
        branch = branch->rc;
        while(branch->lc != NULL) branch=branch->lc;
    }
    return branch;
}

 /**
  * @brief Goes through a category finding the movies that have an average
  *        score higher than the given score and adds them to the array
  *
  * @param node Movie to be examined
  * @param score The minimum score to be added to the array
  * @param array The array of filtered movies
  * @param current_size Active size of the array
  */
 void fillnumMovies(movie_t* node, int score, movie_t** array, int* current_size){
    if(node == NULL) return;
    fillnumMovies(node->lc, score, array, current_size);

    if(node->watchedCounter != 0){ //If it hasn't been watched we can't calculate the average score
        if((node->sumScore / node->watchedCounter) > score){
            array[*current_size] = node;
            *current_size = *current_size +1;
        }
    }

    fillnumMovies(node->rc, score, array, current_size);
}

 /**
  * @brief Goes through the category and counts how many movies have an average score higher than the given score
  *
  * @param numMovies Call by reference to numMovies so we can increase its value
  * @param node Pointer to the current node
  * @param score Score given by the user
  */
 void findnumMovies(int* numMovies, movie_t* node, int score){
    if(node == NULL) return;

    if(node->watchedCounter != 0){
        if((node->sumScore / node->watchedCounter) > score){
            *numMovies = *numMovies +1;
        }
    }

    findnumMovies(numMovies, node->lc, score);
    findnumMovies(numMovies, node->rc, score);
}

 /**
  * @brief Goes through the new_releases tree InOrder, and creates an array for each category
  *        containing all the category's movies in increasing order
  *
  * @param tmp_array the 6 pointers to the 6 category arrays
  * @param array_size the size of each array
  * @param node Current node in the new_releases tree
  */
 void create_category_arrays(movie_t* tmp_array[6], int array_size[6], new_movie_t* node){
    if(node == NULL) return;

    create_category_arrays(tmp_array, array_size, node->lc); //InOrder transversal

    int cat = node->category;
    movie_t *newnode;
    //Create movie_t object and copy data from current node
    array_size[cat]++;
    if(tmp_array[cat] == NULL){
        tmp_array[cat] = malloc(sizeof (movie_t));
        newnode = tmp_array[cat];
    }
    else{
        //If the array isn't empty, add space for a new node
        tmp_array[cat] = realloc(tmp_array[cat], array_size[cat]*sizeof(movie_t));
        newnode = &tmp_array[cat][array_size[cat]-1];
    }
    newnode->movieID = node->movieID; //Copy data
    newnode->watchedCounter = node->watchedCounter;
    newnode->sumScore = node->sumScore;
    newnode->year = node->year;
    newnode->lc = NULL;
    newnode->rc = NULL;

    new_movie_t* rc = node->rc;
    free(node); //Delete node after we copied its data


    create_category_arrays(tmp_array, array_size, rc);
}

 /**
 * @brief Creates a balanced tree from the array (Max height of logn for n nodes)
 *
 * @param array Contains the pointers to the movie_t nodes
 * @param start Start index for Divide and Conquer
 * @param end End index for Divide and Conquer
 * @return The root of the category tree
 */
 movie_t* CreateTree(movie_t* array, int start, int end){
    if(start>end) return NULL;

    int mid = (start + end) / 2;
    movie_t* node = &array[mid];

    node->lc = CreateTree(array, start, mid-1);
    node->rc = CreateTree(array, mid+1, end);

    return node;
}

 /**
  * @brief Calculates which bucket the user needs to be placed in
  *
  * @param userID The user's id
  * @return User's bucket
  */
 unsigned int hash(unsigned int userID){
    return ((hash_a*userID + hash_b) % prime_num)% hashtable_size;
}

 /**
 *
 * @brief Prints the new releases tree
 *
 * @param node The current node
 */
 void print_new_movie_tree(new_movie_t* node){
    if(node == NULL) return;
    print_new_movie_tree(node->lc);
    printf("%d ", node->movieID);
    print_new_movie_tree(node->rc);
}

 /**
 *
 * @brief Prints the tree of a category
 *
 * @param node The current node
 */
 void print_category_tree(movie_t* node){
    if(node == NULL) return;
    print_category_tree(node->lc);
    printf("%d ", node->movieID);
    print_category_tree(node->rc);
}

 /**
 *
 * @brief Prints the history tree of a user (for event W)
 *
 * @param node The current node
 */
 void print_history_tree(userMovie_t * node){
    if(node == NULL) return;
    print_history_tree(node->lc);
    if(node->lc == NULL && node->rc == NULL) printf("   %d %d\n", node->movieID, node->score);
    print_history_tree(node->rc);
}

 /**
 *
 * @brief Prints the history tree of a user (for event P)
 *
 * @param node The current node
 */
 void print_user_history(userMovie_t * node){
    if(node == NULL) return;
    print_user_history(node->lc);
    if(node->lc == NULL && node->rc == NULL) printf("           %d %d\n", node->movieID, node->score);
    print_user_history(node->rc);
}


 //Functions for heap sort
 //I just watched a yt video and used the pseudocode as reference :goat:

 void Heapify(movie_t** array, int i, int n){
     //Find the maximum value
    int left = 2*i;
    int right = 2*i+1;
    int max;
    if((left<= n) && (array[left]->movieID > array[i]->movieID)){
        max = left;
    } else{
        max = i;
    }
    if((right<=n) && (array[right]->movieID > array[max]->movieID)) max = right;
    if(max != i){


        movie_t* tmp;
        tmp = array[i];
        array[i] = array[max];
        array[max] = tmp;
        Heapify(array, max, n);
    }
}

 void BuildMaxHeap(movie_t** array, int n){
     for (int i=n/2; i>=1; i--) Heapify(array,i, n);
 }

 //Goes through the array finding the largest movieID and swapping it with the last node of the current heap
 void HeapSort(movie_t** array, int n){
     BuildMaxHeap(array,n); //Create a Max-Heap
     movie_t* tmp;
     for(int i=n; i>=0; i--){ //Go through all the nodes
         tmp = array[i];
         array[i] = array[1];
         array[1] = tmp;
         n = n-1;
         Heapify(array, 1, n);
     }
 }





 /**
 * @brief Creates a new user.
 * Creates a new user with userID as its identification.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */
 int register_user(int userID){
    if(userID > max_id) return 0;
    //Allocate space for user and assign values
    user_t *new_user = malloc(sizeof(user_t));
    new_user->userID = userID;
    new_user->next = NULL;
    new_user->history = NULL;

    //Calculate the user's bucket using hash
    unsigned int index = hash(userID);

     //If the bucket is empty, assign the user as the first node
     if(user_hashtable_p[index] == NULL){
         user_hashtable_p[index] = new_user;
     }
     //If the bucket already has users, add the new user at the end of the list
     else{
         user_t* tmp = user_hashtable_p[index];
         while(tmp->next != NULL && tmp->userID != userID){
             tmp = tmp->next;
         }
         if(tmp->userID == userID){return 0;}
         tmp->next = new_user;
     }

    printf("R %d\n Chain %d of Users:\n",userID, index);
    user_t* tmp = user_hashtable_p[index];
    while(tmp != NULL){
        printf("    %d\n", tmp->userID);
        tmp = tmp->next;
    }
    printf("DONE\n");
    return 1;
 }
 
 /**
 * @brief Deletes a user.
 * Deletes a user with userID from the system, along with users' history tree.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */
 int unregister_user(int userID){
    if(userID > max_id) return 0;

    //Calculate what that ID's bucket should be using hash
    unsigned int index = hash(userID);

    //Go through the bucket until we find the user or reach end of the list
    user_t* tmp = user_hashtable_p[index];
    user_t* prev = NULL;
    while(tmp != NULL && tmp->userID != userID){
        prev = tmp;
        tmp = tmp->next;
    }

    //If we didn't encounter the user the even was unsuccessful
    if(!tmp){ return 0;}

    //If the user is the first node of the list update the head of the list
    if(prev == NULL){
        user_hashtable_p[index] = tmp->next;
    }
    //If the user is later in the list simply remove them from the list
    else{
        prev->next = tmp->next;
    }

    //Delete the user's watch history as well as their account
    delete_user_history(tmp->history);
    free(tmp);

    printf("U %d\n Chain %d of Users:\n",userID, index);
    tmp = user_hashtable_p[index];
    while(tmp != NULL){
        printf("    %d\n", tmp->userID);
        tmp = tmp->next;
    }
    printf("DONE\n");
    return 1;
 }
 
 /**
 * @brief Add new movie to new release binary tree.
 * Create a node movie and insert it in 'new release' binary tree.
 *
 * @param movieID The new movie identifier
 * @param category The category of the movie
 * @param year The year movie released
 *
 * @return 1 on success
 *         0 on failure
 */
 int add_new_movie(int movieID, int category, int year){
     if(category<0 || category>5) return 0;

     //Search the nre_releases tree for identical movieID
     new_movie_t *tmp = new_releases;
     new_movie_t *prev;
     while(tmp != NULL && tmp->movieID != movieID){
         prev = tmp;
         if(tmp->movieID < movieID){tmp = tmp->rc;}
         else{ tmp = tmp->lc;}
     }

     //The movieID already exists, the event was unsuccessful
     if(tmp != NULL){ return 0;}

     //Create new node for the new movie and assign values
     new_movie_t *newmovie = malloc(sizeof(new_movie_t));
     newmovie->movieID = movieID;
     newmovie->category = category;
     newmovie->year = year;
     newmovie->lc = NULL;
     newmovie->rc = NULL;
     newmovie->watchedCounter = 0;
     newmovie->sumScore = 0;

     //Add it to the sorted binary tree new_releases
     //If the tree is empty make it the root
     if(new_releases == NULL){
         new_releases = newmovie;
     }
     //Otherwise add it as a child
     else{
         if(movieID > prev->movieID){prev->rc = newmovie;}
         else{prev->lc = newmovie;}
     }

     printf("A %d %d %d\nNew releases Tree:\n   new_releases: ", movieID, category, year);
     print_new_movie_tree(new_releases);
     printf("\nDONE\n");
	 return 1;
 }
 
 /**
 * @brief Distribute the movies from new release binary tree to the array of categories.
 *
 * @return 0 on success
 *         1 on failure
 */
 movie_t* tmp_array[6] = { NULL }; //The categorized array (It's global and extern so that I can delete its node afterwords without going through trees like Moglis)
 int distribute_movies(void){
     int array_size[6] = { 0 };

     //Call function that goes through the new_releases tree and creates 6 categorized arrays as well as the nodes
     create_category_arrays(tmp_array, array_size, new_releases);

     printf("\nD\nMovie Category Array:");
     for(int i=0; i<6; i++){
         //Call function for this category that makes a balanced binary tree using the categorized array
         categoryArray[i]->movie = CreateTree(tmp_array[i], 0, array_size[i]-1);
         printf("\n  category%d: ", i);
         print_category_tree(categoryArray[i]->movie);
     }
     printf("\nDONE\n");
	 return 1;
 }
 
 /**
 * @brief User rates the movie with identification movieID with score
 *
 * @param userID The identifier of the user
 * @param category The Category of the movie
 * @param movieID The identifier of the movie
 * @param score The score that user rates the movie with id movieID
 *
 * @return 1 on success
 *         0 on failure
 */
 int watch_movie(int userID,int category, int movieID, int score){
     if(category<0 || category>5) return 0;
     if(score<0 || score>10) return 0;

     //Calculate the user's bucket using hash
     unsigned int index = hash(userID);

     //Go through the bucket until we find the user or reach the end of the list
     user_t* userloc = user_hashtable_p[index];
     while(userloc != NULL && userloc->userID != userID){
         userloc = userloc->next;
     }

     if(!userloc) return 0; //The user doesn't exist, the event was unsuccessful

     //Find the movie's location
     movie_t* movie_loc = categoryArray[category]->movie;
     while(movie_loc != NULL && movie_loc->movieID != movieID){
         if(movie_loc->movieID < movieID){ movie_loc = movie_loc->rc; }
         else{ movie_loc = movie_loc->lc; }
     }

     if(!movie_loc) return 0; //The movie doesn't exist, the event was unsuccessful

     //Update the movie's watch counter and score sum
     movie_loc->watchedCounter++;
     movie_loc->sumScore = movie_loc->sumScore + score;


     //Add the movie to the user's watch history
     //If the user doesn't have a watch history yet
     if(userloc->history == NULL){
         userMovie_t* newnode = malloc(sizeof(userMovie_t));
         newnode->movieID = movie_loc->movieID;
         newnode->category = category;
         newnode->score = score;
         newnode->lc = NULL;
         newnode->rc = NULL;
         userloc->history = newnode;
         newnode->parent = NULL;
     }
     //If the user has a watch history, find the appropriate location for the new node
     else{
         userMovie_t* current = userloc->history;
         userMovie_t* parent = NULL;
         while (current != NULL) {
             parent = current;
             if (movieID <= current->movieID) {
                 current = current->lc;
             } else {
                 current = current->rc;
             }
         }

         //If the movie has a higher mid than its parent, add it as the right child and make a duplicate of the father as a left child
         if(parent->movieID < movieID){
             userMovie_t* left = malloc(sizeof(userMovie_t));
             left->movieID = parent->movieID;
             left->parent = parent;
             left->score = parent->score;
             left->category = parent->category;
             left->rc = NULL;
             left->lc = NULL;
             parent->lc = left;

             userMovie_t* right = malloc(sizeof(userMovie_t));
             right->movieID = movieID;
             right->score = score;
             right->category = category;
             right->parent = parent;
             right->lc = NULL;
             right->rc = NULL;
             parent->rc = right;
         }
         //If the movie has a lower mid than its parent, make it the new parent, assign the previous parent as its right child,
         //make a duplicate of the new parent as the left child and update the parent's parent with the new parent
         else{
             userMovie_t* newparent = malloc(sizeof(userMovie_t));
             newparent->movieID = movieID;
             newparent->category = category;
             newparent->score = score;
             newparent->parent = parent->parent;
             newparent->rc = parent;


             //Update the  previous parent's parent with the new parent
             if(parent->parent != NULL){
                 if(parent->parent->rc == parent){
                     parent->parent->rc = newparent;
                 } else{
                     parent->parent->lc = newparent;
                 }
             }
             //If the  previous parent was the root, update the pointer to head of the user
             else{
                 userloc->history = newparent;
             }

             parent->parent = newparent;

             userMovie_t* left = malloc(sizeof(userMovie_t));
             left->movieID = movieID;
             left->parent = newparent;
             left->score = score;
             left->category = category;
             left->rc = NULL;
             left->lc = NULL;
             newparent->lc = left;
         }


     }

     printf("W %d %d %d %d\nHistory Tree of user %d:\n", userID, category, movieID, score, userID);
     print_history_tree(userloc->history);
     printf("DONE\n");
	 return 1;
 }
 
 /**
 * @brief Identify the best rating score movie and cluster all the movies of a category.
 *
 * @param userID The identifier of the user
 * @param score The minimum score of a movie
 *
 * @return 1 on success
 *         0 on failure
 */
 int filter_movies(int userID, int score){
     if(score<0 || score>10) return 0;

    int i;

    //Calculate the bucket the user should be in using hash
    unsigned int index = hash(userID);

    //Go through the bucket's list until we find the user or reach the end of the list
    user_t* userloc = user_hashtable_p[index];
    while(userloc != NULL && userloc->userID != userID){
        userloc = userloc->next;
    }

    if(!userloc) return 0; //The user doesn't exist, the event was unsuccessful

    //Find the number of movies with average score higher than requested by the user
    int numMovies = 0;
    for (i=0; i < 6; i++) {
        findnumMovies(&numMovies, categoryArray[i]->movie, score);
    }

    //If we couldn't find any movies the event was unsuccessful
    if(numMovies == 0) return 0;

    //Create an array for the movies found
    movie_t** array = malloc(numMovies * sizeof(movie_t*));
    int current_size = 0;

    //Add the movies to the array
    for(i=0; i<6; i++){
        fillnumMovies(categoryArray[i]->movie, score, array, &current_size);
    }

    //Sort the array
    HeapSort(array, current_size-1);

    printf("F %d %d\n   ", userID, score);
    for (i=0; i<numMovies; i++) {
        printf("%d %d  ", array[i]->movieID, (array[i]->sumScore/array[i]->watchedCounter));
    }
    printf("\nDONE\n");

    //Free the memory for the array and exit
    free(array);
    return 1;
}
 
 /**
 * @brief Find movies from categories withn median_score >= score t
 *
 * @param userID The identifier of the user
 * @param category Array with the categories to search.
 * @param score The minimum score the movies we want to have
 *
 * @return 1 on success
 *         0 on failure
 */
 int user_stats(int userID){
     //Calculate the bucket the user should be in using hash
     unsigned int index = hash(userID);

     //Go through the bucket's list until we find the user or reach the end of the list
     user_t* userloc = user_hashtable_p[index];
     while(userloc != NULL && userloc->userID != userID){
         userloc = userloc->next;
     }

     if(!userloc) return 0; //The user doesn't exist, the event was unsuccessful

    userMovie_t* tmp = userloc->history;

    if(tmp == NULL) return 0;//If the user doesn't have a watch history the event was unsuccessful

    //Go through the user's watch history
    while(tmp->lc != NULL){
        tmp = tmp->lc;
    }
    int ScoreSum = tmp->score;
    int counter = 1;

    //Go through all the leafs of the tree using the FindNextLeaf function
    tmp = FindNextLeaf(tmp);
    while(tmp != NULL){
        counter++;
        ScoreSum = ScoreSum + tmp->score;
        tmp = FindNextLeaf(tmp);
    }

    printf("Q %d %f\nDONE\n", userloc->userID, (float)ScoreSum/counter);
    return 1;
 }
 
 /**
 * @brief Search for a movie with identification movieID in a specific category.
 *
 * @param movieID The identifier of the movie
 * @param category The category of the movie
 *
 * @return 1 on success
 *         0 on failure
 */
 int search_movie(int movieID, int category){
     if(category<0 || category>5) return 0;

     //Search the movie in the given category
     movie_t* tmp = categoryArray[category]->movie;
     while(tmp != NULL && tmp->movieID != movieID){
         if(tmp->movieID < movieID){ tmp = tmp->rc; }
         else{ tmp = tmp->lc; }
     }

     if(!tmp) return 0; //If we couldn't find the movie in the category the event was unsuccessful

     //Print the movie's year of release
    printf("I %d %d %d\nDONE\n",tmp->movieID, category, tmp->year);
    return 1;
 }

 /**
 * @brief Prints the movies in movies categories array.
 * @return 1 on success
 *         0 on failure
 */
 int print_movies(void){
     printf("M\nMovie Category Array:");
     for(int i=0; i<6; i++){
         printf("\n  category%d: ", i);
         print_category_tree(categoryArray[i]->movie);
     }
     printf("\nDONE\n");


	 return 1;
 }
 
 /**
 * @brief Prints the users hashtable.
 * @return 1 on success
 *         0 on failure
 */
 int print_users(void){
      printf("P\n");
      user_t* tmp;
      for(int i=0; i<hashtable_size; i++){
          printf("Chain %d of Users:\n", i);
          tmp = user_hashtable_p[i];
          while(tmp != NULL){
              printf("      %d\n      History Tree:\n", tmp->userID);
              print_user_history(tmp->history);
              tmp = tmp->next;
          }
      }
      printf("DONE\n");
	  return 1;
 }