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

//struct to define a single pixel in the image.
typedef struct pixel {
    float red;
    float green;
    float blue;
} pixel_t;

//node to describea moment with the mouse clicked on the window, with its transformed position and its state. (start of stroke, path or end)
typedef struct vector2_node {

    vector2_t vector;
    struct vector2_node *next;
    node_state_t state;
    pixel_t color;
    float brush_size;

} vector2_node_t;

//Head of linked list for strokes
vector2_node_t *head;
vector2_node_t *last;

//prototypes

void clear_screen(void);
gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
void get_window_size_on_change(GtkWidget *widget, GtkAllocation *allocation, void *data);
gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data);
void paint_window(cairo_t *cr);

void size_slider_moved(GtkRange *range, gpointer user_data);
gboolean dw_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean dw_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data);
gboolean dw_keyPressed(GtkWidget *widget, GdkEventKey *event, gpointer data);
gboolean dw_mousewheel(GtkWidget *widget, GdkEventScroll *event, gpointer data);

vector2_t pixel_to_vpos(pixel_vector_t pixel);
pixel_vector_t vpos_to_pixel(vector2_t vpos);

void close_window(void);
void unload(void);
void activate(GtkApplication *app, gpointer user_data);