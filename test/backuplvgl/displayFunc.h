#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

static const uint16_t screenWidth = TFT_HEIGHT;
static const uint16_t screenHeight = TFT_WIDTH;

static void event_handler(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED)
  {
    LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
  }
}

void lv_example_switch_1(void)
{
  lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lv_obj_t *sw;

  sw = lv_switch_create(lv_scr_act());
  lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);

  sw = lv_switch_create(lv_scr_act());
  lv_obj_add_state(sw, LV_STATE_CHECKED);
  lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);

  sw = lv_switch_create(lv_scr_act());
  lv_obj_add_state(sw, LV_STATE_DISABLED);
  lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);

  sw = lv_switch_create(lv_scr_act());
  lv_obj_add_state(sw, LV_STATE_CHECKED | LV_STATE_DISABLED);
  lv_obj_add_event_cb(sw, event_handler, LV_EVENT_ALL, NULL);
}

static lv_obj_t *chart1;
static lv_chart_series_t *ser1;
static lv_chart_series_t *ser2;

static void draw_event_cb(lv_event_t *e)
{
  lv_obj_t *obj = lv_event_get_target(e);

  /*Add the faded area before the lines are drawn*/
  lv_obj_draw_part_dsc_t *dsc = lv_event_get_draw_part_dsc(e);
  if (dsc->part == LV_PART_ITEMS)
  {
    if (!dsc->p1 || !dsc->p2)
      return;

    /*Add a line mask that keeps the area below the line*/
    lv_draw_mask_line_param_t line_mask_param;
    lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                  LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

    /*Add a fade effect: transparent bottom covering top*/
    lv_coord_t h = lv_obj_get_height(obj);
    lv_draw_mask_fade_param_t fade_mask_param;
    lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                           obj->coords.y2);
    int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

    /*Draw a rectangle that will be affected by the mask*/
    lv_draw_rect_dsc_t draw_rect_dsc;
    lv_draw_rect_dsc_init(&draw_rect_dsc);
    draw_rect_dsc.bg_opa = LV_OPA_20;
    draw_rect_dsc.bg_color = dsc->line_dsc->color;

    lv_area_t a;
    a.x1 = dsc->p1->x;
    a.x2 = dsc->p2->x - 1;
    a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
    a.y2 = obj->coords.y2;
    lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

    /*Remove the masks*/
    lv_draw_mask_free_param(&line_mask_param);
    lv_draw_mask_free_param(&fade_mask_param);
    lv_draw_mask_remove_id(line_mask_id);
    lv_draw_mask_remove_id(fade_mask_id);
  }
  /*Hook the division lines too*/
  else if (dsc->part == LV_PART_MAIN)
  {
    if (dsc->line_dsc == NULL || dsc->p1 == NULL || dsc->p2 == NULL)
      return;

    /*Vertical line*/
    if (dsc->p1->x == dsc->p2->x)
    {
      dsc->line_dsc->color = lv_palette_lighten(LV_PALETTE_GREY, 1);
      if (dsc->id == 3)
      {
        dsc->line_dsc->width = 2;
        dsc->line_dsc->dash_gap = 0;
        dsc->line_dsc->dash_width = 0;
      }
      else
      {
        dsc->line_dsc->width = 1;
        dsc->line_dsc->dash_gap = 6;
        dsc->line_dsc->dash_width = 6;
      }
    }
    /*Horizontal line*/
    else
    {
      if (dsc->id == 2)
      {
        dsc->line_dsc->width = 2;
        dsc->line_dsc->dash_gap = 0;
        dsc->line_dsc->dash_width = 0;
      }
      else
      {
        dsc->line_dsc->width = 2;
        dsc->line_dsc->dash_gap = 6;
        dsc->line_dsc->dash_width = 6;
      }

      if (dsc->id == 1 || dsc->id == 3)
      {
        dsc->line_dsc->color = lv_palette_main(LV_PALETTE_GREEN);
      }
      else
      {
        dsc->line_dsc->color = lv_palette_lighten(LV_PALETTE_GREY, 1);
      }
    }
  }
}

static void add_data(lv_timer_t *timer)
{
  LV_UNUSED(timer);
  static uint32_t cnt = 0;
  lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));

  if (cnt % 4 == 0)
    lv_chart_set_next_value(chart1, ser2, lv_rand(40, 60));

  cnt++;
}

void lv_example_chart_2(void)
{
  /*Create a chart1*/
  chart1 = lv_chart_create(lv_scr_act());
  // lv_obj_set_size(chart1, 200, 150);
  lv_obj_center(chart1);
  lv_chart_set_type(chart1, LV_CHART_TYPE_LINE); /*Show lines and points too*/

  lv_chart_set_div_line_count(chart1, 5, 7);

  lv_obj_add_event_cb(chart1, draw_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
  lv_chart_set_update_mode(chart1, LV_CHART_UPDATE_MODE_CIRCULAR);
  lv_obj_set_size(chart1, screenWidth, screenHeight);
  /*Add two data series*/
  ser1 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
  ser2 = lv_chart_add_series(chart1, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);

  uint32_t i;
  for (i = 0; i < 10; i++)
  {
    lv_chart_set_next_value(chart1, ser1, lv_rand(20, 90));
    lv_chart_set_next_value(chart1, ser2, lv_rand(30, 70));
  }

  lv_timer_create(add_data, 200, NULL);
}
static lv_obj_t *arc;

static void value_changed_event_cb(lv_event_t *e);

void arc_1(void)
{
  // lv_obj_t * label = lv_label_create(lv_scr_act());

  /*Create an Arc*/
  arc = lv_arc_create(lv_scr_act());
  int adj_arc = -20;
  lv_obj_set_size(arc, screenWidth + adj_arc, screenHeight + adj_arc);
  // lv_arc_set_rotation(arc, 360);
  lv_arc_set_bg_angles(arc, 0, 360);
  lv_obj_remove_style(arc, NULL, LV_PART_INDICATOR); /*Be sure the knob is not displayed*/
  lv_arc_set_value(arc, 50);
  lv_obj_align(arc, LV_ALIGN_CENTER, 20, 0);
  // lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

  /*Manually update the label for the first time*/
  lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
}

void lv_example_btn_grid(void)
{
    static lv_coord_t col_dsc[] = {85, 85, 85, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {55, 55, 55, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 310, 225);
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

    lv_obj_t * label;
    lv_obj_t * obj;

    uint32_t i;
    for(i = 0; i < 9; i++) {
        uint8_t col = i % 3;
        uint8_t row = i / 3;

        obj = lv_btn_create(cont);
        /*Stretch the cell horizontally and vertically too
         *Set span to 1 to make the cell 1 column/row sized*/
        lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1,
                             LV_GRID_ALIGN_STRETCH, row, 1);
        
        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "c%d, r%d", col, row);
        lv_obj_center(label);
    }
    lv_obj_center(obj);
}