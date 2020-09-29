#include <stdint.h>
#include <gtk/gtk.h>
#include "gtkdraw.h"

//surface for drawing.
static cairo_surface_t *surface = NULL;

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
/* clear the screen. */
static void clear_surface(void)
{
    cairo_t *cr;

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_destroy(cr);
}

/* create new surface to store drawings, with appropriate size. */
static gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
    if (surface)
        cairo_surface_destroy(surface);

    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
                                                CAIRO_CONTENT_COLOR,
                                                gtk_widget_get_allocated_width(widget),
                                                gtk_widget_get_allocated_height(widget));

    /* Initialize the surface to white */
    clear_surface();

    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}

/* draw surface to the screen */
static gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    return FALSE;
}

/* called when mouse is clicked; does not get called each frame, but rather once when you press the mouse. */
static gboolean dw_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    //in case something went horribly wrong
    if (surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY) {
        vector2_node_t *StartNode = malloc(sizeof(vector2_node_t));
        //set positions and initialize .next
        StartNode->vector.x = event->x;
        StartNode->vector.y = event->y;
        StartNode->next = NULL;

        //if this is the first click: point both head & last to the new node.
        //plus, set the state to start & end, as we dont have an end node yet.
        if (head == NULL) {
            head = StartNode;
            last = StartNode;
            StartNode->state = start_and_end;
        //else: the list is not empty, so we append to the end, and point currently last node to new node.
        } else {
            last->next = StartNode;
            last = StartNode;
            StartNode->state = start_and_end;
        }
        
    }

    //everything has gone well. (hopefully)
    return TRUE;
}

/* gets called when we already clicked the mouse, but we moved to a different position. */
static gboolean dw_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    //in case something goes wrong
    if (surface == NULL)
        return FALSE;

    if (event->state & GDK_BUTTON1_MASK) {
        vector2_node_t *NewNode = malloc(sizeof(vector2_node_t));

        NewNode->vector.x = event->x;
        NewNode->vector.y = event->y;
        NewNode->next = NULL;

        //whatever happens, we definetly want the last node to point at us.
        last->next = NewNode;

        //if the state of the last node is start_and_end, then change its state to start.
        //(in other words if this stroke only has the initial click)
        if (last->state == start_and_end)
            last->state = start;
        //We already have more than one node in this stroke, so we can continue it by updating states for last node
        //and append a new node at the end.
        else if (last->state == end) {
            last->state = path;
            NewNode->state = end;
        } else  {
            //something has gone horribly wrong when setting the states up, quickly exit and report the user
            fprintf(stderr, "ERROR: linked list not set up correctly, node had invalid state: %d\n", last->state);
            return FALSE;
        }

        last = NewNode;
        NewNode->state = end;
    }

    //everything went well
    return TRUE;
}

/* make sure to destroy cairo surface when we close the window */
static void close_window(void) {
    if (surface)
        cairo_surface_destroy(surface);
}

/* activate window stuff. Very similar to java constructor when using the java window stuff. */
static void activate(GtkApplication *app, gpointer user_data) {
    //simple elements: a window, a frame, and the drawing area for the strokes.
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;

    //create new window, and set its title.
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Drawing Area");

    //connect signal for when we close the window, the cairo surface gets destroyed as well.
    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    //set border width for the window. (to leave some space between draw area and window)
    gtk_container_set_border_width(GTK_CONTAINER(window), 8);

    //create new frame, no label. (NULL)
    frame = gtk_frame_new(NULL);
    //set the shadow for the frame
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    //add frame to window.
    gtk_container_add(GTK_CONTAINER(window), frame);

    //create new draw area
    drawing_area = gtk_drawing_area_new();
    //set minimum size
    gtk_widget_set_size_request(drawing_area, 100, 100);
    //add draw area to the frame
    gtk_container_add(GTK_CONTAINER(frame), drawing_area);

    //Connect signal to draw the surface to the screen
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_cb), NULL);
    //Connect signal to create a new surface with the appropriate size
    g_signal_connect(drawing_area, "configure-event", G_CALLBACK(configure_event_cb), NULL);

    //connect signals for mouse events
    g_signal_connect(drawing_area, "motion-notify-event", G_CALLBACK(dw_moved), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(dw_clicked), NULL);

    //set events
    gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);

    //show widgets; otherwise nothing will happen
    gtk_widget_show_all(window);
}

