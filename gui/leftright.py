import gtk
import math
import gobject

class leftright(gtk.DrawingArea):
	def init(self):
		self.connect("expose-event", self.expose)
		self.set_events(gtk.gdk.BUTTON_PRESS_MASK)
		self.connect('button-press-event', self.on_drawing_area_button_press)
		col = gtk.gdk.Color('#fff')
		self.modify_bg(gtk.STATE_NORMAL, col)
		self.value=False
		self.h=20
		self.set_size_request(25,self.h)

	def rounded_rectangle(self,cr, x, y, w, h, r=20):
		cr.move_to(x+r,y)
		cr.line_to(x+w-r,y)
		cr.curve_to(x+w,y,x+w,y,x+w,y+r)
		cr.line_to(x+w,y+h-r)
		cr.curve_to(x+w,y+h,x+w,y+h,x+w-r,y+h)
		cr.line_to(x+r,y+h)
		cr.curve_to(x,y+h,x,y+h,x,y+h-r)
		cr.line_to(x,y+r)
		cr.curve_to(x,y,x,y,x+r,y)

	def expose(self, widget, event):
		self.switch_width=100
		self.offset=7
		cr = widget.window.cairo_create()
		self.style.set_background(widget.window, gtk.STATE_NORMAL)
		self.rounded_rectangle(cr, 5, 5, 100, 100, r=4)
		cr.set_source_rgba(0.0,0.0,0.0,0)
		cr.fill()
		cr.set_line_width(3)
				
		w = self.allocation.width
		h = self.allocation.height
		cr.translate(1, 1)

		cr.set_source_rgb(0.1, 0.1, 0.1)
		self.rounded_rectangle(cr, 5, 5, self.switch_width, self.h, r=4)
		cr.stroke_preserve()
		if self.value==True:
			cr.set_source_rgb(0.8, 0.8, 0.8)
		else:
			cr.set_source_rgb(0.8, 0.8, 0.8)
		cr.fill()

		cr.set_font_size(14)
		if self.value==True:
			cr.set_line_width(1)
			cr.set_source_rgb(0.1, 0.1, 0.1)
			self.rounded_rectangle(cr, self.switch_width/2+3, 7, self.switch_width/2, self.h-4, r=4)	#button
			cr.stroke_preserve()
			cr.set_source_rgb(0.9, 0.9, 0.9)
			cr.fill()
			cr.set_source_rgb(0.0, 0.0, 0.0)
			cr.move_to(10, self.h-1)
			cr.show_text("Right")
			#cr.paint()

		else:

			cr.set_line_width(1)
			cr.set_source_rgb(0.1, 0.1, 0.1)
			self.rounded_rectangle(cr, self.offset, self.offset, self.switch_width/2, self.h-4, r=4)
			cr.stroke_preserve()
			cr.set_source_rgb(0.9, 0.9, 0.9)
			cr.fill()
			cr.set_source_rgb(0.0, 0.0, 0.0)
			cr.move_to(self.switch_width/2+20, self.h-1)
			cr.show_text("Left")

	def get_active_text(self):
		if self.value==True:
			return "right"
		else:
			return "left"

	def set_value(self,value):
		self.value=value
		self.queue_draw()

	def on_drawing_area_button_press(self, widget, event):
		if event.type == gtk.gdk.BUTTON_PRESS:
			if event.x<(self.switch_width+self.offset) and event.y<35:
				self.value= not self.value
				self.queue_draw()
				self.emit("changed")
		return True

gobject.type_register(leftright)
gobject.signal_new("changed", leftright, gobject.SIGNAL_RUN_FIRST, gobject.TYPE_NONE, ())
