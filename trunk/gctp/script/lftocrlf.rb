Dir.glob("*.html") {|file|
	w = File.readlines(file)
	w.collect! {|l|
		l.chomp + "\n"
	}
	f = open(file, "w")
	f << w
	f.close
}
