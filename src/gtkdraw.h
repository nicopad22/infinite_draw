#include <stdint.h>
#include <gtk/gtk.h>

//states for each node for when mouse is pressed; used when drawing the lines onto the screen.
typedef enum node_state {
    start,
    end,
    path,
    start_and_end
} node_state_t;

//basic struct to describe accurate position, not related to pixels.
typedef struct vector2 {
    double x;
    double y;
} vector2_t;

//x,y pair for pixels.
typedef struct pixel_vector {
    int x;
    int y;
} pixel_vector_t;

//node to describea moment with the mouse clicked on the window, with its transformed position and its state. (start of stroke, path or end)
typedef struct vector2_node {
    vector2_t vector;
    struct vector2_node *next;
    node_state_t state;
} vector2_node_t;

//struct to define a single pixel in the image.
typedef struct pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

//Head of linked list for strokes
vector2_node_t *head;
vector2_node_t *last;

//prototypes

static void clear_screen(void);
static gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean dw_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data);
static gboolean dw_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data);
static void paint_window(cairo_t *cr);
static void close_window(void);
static void activate(GtkApplication *app, gpointer user_data);