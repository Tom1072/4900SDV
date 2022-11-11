#pragma once
#include "commons.h"

// Public APIs

/**
 * @brief Initialize the view
 * This function must be called before any other view functions
 * 
 */
void initView();

/**
 * @brief Destroy the view, free the memory
 * This function must be called after the view is no longer needed
 * 
 */
void destroyView();

/**
 * @brief Set the Environment object
 * This will only set the attributes that are not NULL
 * Ex: If env->skid is NULL, then the current environment->skid will not be changed
 * 
 * @param env 
 */
void setEnvironment(Environment* env);


// Private APIs

/**
 * @brief Get the User Input
 *
 * @return char*
 */
char *getUserInput();

/**
 * @brief Update the view
 *
 */
void renderView();