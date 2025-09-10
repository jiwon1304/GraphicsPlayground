/*
 * glfw_fwd.hpp
 *
 * Forward declarations for frequently used GLFW opaque (or semi‑opaque) handle types.
 *
 * WHY:
 *   - To reduce compile-time coupling: many of your headers only need to store
 *     pointers (GLFWwindow*, GLFWmonitor*, GLFWcursor*) and don't require the full
 *     <GLFW/glfw3.h> inclusion.
 *
 * LIMITATIONS:
 *   - These are incomplete types; you may ONLY:
 *       * Declare pointers/references to them
 *       * Compare pointer values
 *       * Pass them to functions that are declared elsewhere
 *   - You CANNOT:
 *       * Define instances (e.g., `GLFWwindow win;`) — size is unknown
 *       * Access members (there are none exposed; they are opaque anyway)
 *       * Use sizeof / alignof on them
 *
 * NOTES:
 *   - GLFW is a C library; there are no C++ "classes" here. The common usage pattern
 *     in C++ code treats these opaque structs like handle types.
 *   - If you need anything beyond pointer storage (e.g., to call glfwCreateWindow),
 *     include <GLFW/glfw3.h> in the corresponding .cpp file.
 *   - Do NOT forward declare non-opaque structs that you intend to dereference
 *     (e.g., GLFWvidmode, GLFWgamepadstate) — you must include the real header
 *     before using their fields.
 *
 * SAFE PATTERN:
 *   Header (.hpp):
 *       #include "glfw_fwd.hpp"
 *       class MyApp {
 *           GLFWwindow* window_{};
 *       };
 *
 *   Implementation (.cpp):
 *       #include <GLFW/glfw3.h>
 *       #include "MyApp.hpp"
 *       // use glfw* functions here
 *
 * VERSION COMPATIBILITY:
 *   These forward declarations are stable across GLFW 3.x because the names
 *   and opaque nature of these structs are part of the public API.
 */

#pragma once

// Forward declarations (opaque handles)
struct GLFWwindow;
struct GLFWmonitor;
struct GLFWcursor;
