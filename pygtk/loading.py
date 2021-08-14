import sys
import config
import os.path

global have_gtk
have_gtk = False

if not sys.executable.endswith("pythonw.exe"):
	print("PYTHON PATH =",sys.path)

try:
	import gi 
	gi.require_version('Gtk', '3.0') 
	from gi.repository import Gtk
	have_gtk = True
except Exception as e:

	if sys.platform=="win32":
		try:
			from ctypes import c_int, WINFUNCTYPE, windll
			from ctypes.wintypes import HWND, LPCSTR, UINT
			prototype = WINFUNCTYPE(c_int, HWND, LPCSTR, LPCSTR, UINT)
			paramflags = (1, "hwnd", 0), (1, "text", "Hi"), (1, "caption", None), (1, "flags", 0)
			MessageBox = prototype(("MessageBoxA", windll.user32), paramflags)
			MessageBox(text="""ASCEND could not load PyGI. Probably this is because
either PyGI, PyCairo, PyGObject or GTK3+ are not installed on your
system. Please try re-installing ASCEND to rectify the problem.""")
		except:	
			pass
	else:
		print("PyGI COULD NOT BE LOADED (is it installed? do you have X-Windows running?) (%s)" % str(e))
		
	sys.exit("FATAL ERROR: PyGI not available, unable to start ASCEND.")

global _messages
_messages = []

def get_messages():
	return _messages

def load_matplotlib(throw=False,alert=False):
	print_status("Loading python matplotlib")
	try:
		import matplotlib
        # Added a new backend backend_gtk3. File bundle exists in PYTHONPATH
		matplotlib.use('module://backend_gtk3')
		try:
			print_status("Trying python numpy")
			import numpy
			print_status("","Using python module numpy")
		except ImportError:
			print_status("","FAILED to load Python module 'numpy'")
		import pylab


	except ImportError as e:
		print_status("","FAILED TO LOAD MATPLOTLIB")
		if alert or throw:
			_d = Gtk.MessageDialog(None,Gtk.DialogFlags.MODAL | Gtk.DialogFlags.DESTROY_WITH_PARENT
				,Gtk.MessageType.ERROR,Gtk.ButtonsType.CLOSE,"Plotting functions are not available unless you have 'matplotlib' installed.\n\nSee http://matplotlib.sf.net/\n\nFailed to load matplotlib (%s)" % str(e)
			)
			_d.run()
			_d.destroy()
			while Gtk.events_pending():
				Gtk.main_iteration_do(False)		
		if throw:
			raise RuntimeError("Failed to load plotting library 'matplotlib'. (%s)" % str(e))


class LoadingWindow:
	def __init__(self):
		self.is_loading = False
		self.set_assets_dir(config.PYGTK_ASSETS)

	def set_assets_dir(self, d):
		self.assetsdir = d
		self.splashfile = os.path.join(self.assetsdir,'ascend-loading.png')

	def create_window(self):
		if have_gtk:
			if os.path.exists(self.splashfile):
				_w = Gtk.Window(Gtk.WindowType.TOPLEVEL)
				_w.set_decorated(False)
				_w.set_position(Gtk.WindowPosition.CENTER)
				_a = Gtk.Alignment.new(0.5,0.5,0,0)
				_a.set_padding(4,4,4,4)
				_w.add(_a)
				_a.show()
				_v = Gtk.VBox()
				_a.add(_v)
				_v.show()
				_i = Gtk.Image()
				self.image = _i
				_i.set_pixel_size(3)
				_i.set_from_file(self.splashfile)
				_v.add(_i)
				_i.show()
				_l = Gtk.Label(label="Loading ASCEND...")
				_l.set_justify(Gtk.Justification.CENTER)
				_v.add(_l)
				_l.show()
				_w.show()
				self.window = _w
				self.label = _l
				self.is_loading = True
				while Gtk.events_pending():
					Gtk.main_iteration()
			else:
				pass
				#do nothing, don't know where splash file is yet
		else:
			print("DON'T HAVE GTK!")
			sys.exit(1)
	
	def print_status(self,status,msg=None):
		if self.is_loading:
			if not sys.executable.endswith("pythonw.exe"):
				print(status)
			self.label.set_text(status)
			if msg is not None:
				try:
					sys.stderr.write(msg+"\n")
				except IOError:
					pass
				_messages.append(msg)
			while Gtk.events_pending():
				Gtk.main_iteration()
		else:
			try:
				sys.stderr.write("\r                                                 \r")
				if msg!=None:
					sys.stderr.write(msg+"\n")
					_messages.append(msg)
				sys.stderr.write(status+"...\r")
				sys.stderr.flush()
			except IOError:
				pass

	def complete(self):
		if self.is_loading:
			self.window.destroy()
		self.is_loading = False

global w

def print_status(status,msg=None):
	w.print_status(status,msg)

def complete():
	w.complete()

def create_window(assetsdir=config.PYGTK_ASSETS):
	w.set_assets_dir(assetsdir)
	w.create_window()

w = LoadingWindow()
create_window()
