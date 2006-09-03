Dir.glob("gc*.hpp gc*.cpp") {|file|
	w = File.readlines(file)
	w.each() {|l|
		l.gsub!("(gc.*\.h)", '\1pp')
	}
	f = open(file, "w")
	f << w
	f.close
}
