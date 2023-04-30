package main

import (
	"fmt"
	"github.com/nfnt/resize"
	"image"
	"image/gif"
	_ "image/jpeg"
	_ "image/png"
	"os"
	"os/exec"
	"path/filepath"
)

func checkErr(err error) {
	if err != nil {
		fmt.Printf("err: %v\n", err)
		panic(err)
	}
}

const MaxW uint = 500
const MaxH uint = 500

func main() {

	source := parseArg(os.Args)
	if !exists(source) {
		fmt.Println("没有找到图片: " + source)
		return
	}
	file, err := os.Open(source)
	outFile := openOrCreate("./1.gif")
	imgData, _, err := image.Decode(file)
	bounds := imgData.Bounds()
	checkErr(err)
	if bounds.Dx() > bounds.Dy() {
		imgData = resize.Resize(MaxW, 0, imgData, resize.Lanczos3)
	} else {
		imgData = resize.Resize(0, MaxH, imgData, resize.Lanczos3)
	}
	checkErr(err)
	err = gif.Encode(outFile, imgData, nil)
	checkErr(err)
	cmd := "clip.exe " + getPath() + "\\1.gif"
	//fmt.Println("cmd: " + cmd)
	_, err = exec.Command("cmd", "/c", cmd).Output()
	checkErr(err)
}

func openOrCreate(filename string) *os.File {
	file, err := os.Create(filename)
	checkErr(err)
	return file
}

func exists(filename string) bool {
	if _, err := os.Stat(filename); os.IsNotExist(err) {
		return false
	}
	return true
}

func parseArg(arg []string) string {
	if len(arg) < 2 {
		fmt.Println("没有输入原图片地址")
		fmt.Println("用法: ./toGif source.png")
		fmt.Println()
		panic("")
	}
	return arg[1]
}

func getPath() string {
	// 获取执行文件的路径
	file, err := exec.LookPath(os.Args[0])
	if err != nil {
		panic(err)
	}

	// 获取执行文件所在的目录的路径
	dir, err := filepath.Abs(filepath.Dir(file))
	if err != nil {
		panic(err)
	}
	return dir
}
