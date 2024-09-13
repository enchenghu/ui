import cv2
import os
import glob
import argparse

def Img2Video(img_dir, out_path):
    fps = 10
    files = glob.glob(os.path.join(img_dir, '*.png'))
    files = sorted(files, key=lambda p: int(os.path.basename(p).split('/')[-1].split('.')[0]))
    height, width, _ = cv2.imread(files[0]).shape
    height = int(height / 2)
    width = int(width / 2)
    fourcc = cv2.VideoWriter_fourcc(*'DIVX')
    videoWriter = cv2.VideoWriter(out_path, fourcc, fps, (width, height))

    for i in range(len(files)):
        frame = cv2.imread(files[i])
        frame.resize((height, width, 3))
        videoWriter.write(frame)

    videoWriter.release()

if __name__ == "__main__":
    argparser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    argparser.add_argument(
        "-i",
        "--input",
        type=str,
        help="image directory",
        required=True
    )
    argparser.add_argument("-o", "--output", help="Output video path (file extension name: .avi)", type=str, default="./video.avi")

    args = argparser.parse_args()
    Img2Video(args.input, args.output)