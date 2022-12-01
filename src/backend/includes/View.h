#pragma once
#include "commons.h"

/**
 * Display the current state of the SDV to stdout
 * 
 */

// Public APIs

/**
 * @brief Initialize the view
 * This function must be called before any other view functions
 * 
 */
void init_view();

/**
 * @brief Destroy the view, free the memory
 * This function must be called after the view is no longer needed
 * 
 */
void destroy_view();

/**
 * @brief Set the Environment object
 * This will only set the attributes that are not NULL
 * Ex: If env->skid is NULL, then the current environment->skid will not be changed
 * 
 * @param env The new environment data
 */
void set_environment(Environment* env);


// Private APIs
/**
 * @brief Print the current environment
 *
 */
void render_view();