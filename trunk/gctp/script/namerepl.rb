Dir.glob("gc*.h") {|file|
	File.rename(file, file.gsub("\.h", ".hpp")) 
}
