from pathlib import Path
from PIL import Image, ImageDraw, ImageFont


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
OUTPUT = HERE / "report-preview.png"
CHART = ROOT / "resources" / "images" / "age_sos_woman.bmp"


def font(size: int, bold: bool = False):
    name = "msyhbd.ttc" if bold else "msyh.ttc"
    return ImageFont.truetype(str(Path("C:/Windows/Fonts") / name), size)


def text(draw, xy, value, size=14, color="#303133", bold=False, anchor=None):
    draw.text(xy, value, font=font(size, bold), fill=color, anchor=anchor)


canvas = Image.new("RGB", (1120, 1070), "#e8edf3")
draw = ImageDraw.Draw(canvas)

# Report sheet
page = (22, 18, 762, 1052)
draw.rounded_rectangle(page, radius=7, fill="white", outline="#1296e8", width=9)
draw.rectangle((27, 23, 757, 68), fill="#1296e8")
text(draw, (46, 44), "医疗参考", 24, "white", True, "lm")

# Header icon and title
cx, cy = 270, 105
draw.rounded_rectangle((cx - 25, cy - 25, cx + 25, cy + 25), radius=11, fill="#ef1f2f")
draw.rectangle((cx - 16, cy - 6, cx + 16, cy + 6), fill="white")
draw.rectangle((cx - 6, cy - 16, cx + 6, cy + 16), fill="white")
text(draw, (440, 105), "超声骨密度检测报告", 28, "#1f2329", True, "mm")

left, right = 56, 730

def section(y, title):
    text(draw, (left, y), title, 17, "#606266", True)


section(146, "受检者信息")
draw.rounded_rectangle((left, 172, right, 292), radius=7, fill="#fbfcfe", outline="#d9dee5", width=1)
info = [
    (72, 190, "姓名：", "王某"), (72, 218, "年龄：", "47 岁"),
    (72, 246, "出生年月：", "1979-04-18"), (72, 274, "身高：", "164 cm"),
    (405, 190, "ID：", "P-2026-023"), (405, 218, "性别：", "女"),
    (405, 246, "检测时间：", "2026-09-22 10:26"), (405, 274, "体重：", "56 kg"),
]
for x, y, label, value in info:
    text(draw, (x, y), label, 13, "#909399")
    text(draw, (x + 82, y), value, 14, "#303133")

section(310, "测量结果")
draw.rounded_rectangle((left, 336, right, 406), radius=7, fill="#fbfcfe", outline="#d9dee5", width=1)
metrics = [
    (72, "桡骨 SOS", "4090.0", "m/s", "#0b86d4"),
    (238, "骨强度", "正常", "", "#303133"),
    (405, "T 值", "-0.8", "", "#303133"),
    (570, "Z 值", "-0.3", "", "#303133"),
]
for index, (x, label, value, unit, color) in enumerate(metrics):
    if index:
        draw.line((x - 14, 348, x - 14, 394), fill="#e4e7ed", width=1)
    text(draw, (x, 350), label, 11, "#909399")
    text(draw, (x, 372), value, 19 if index == 0 else 16, color, True)
    if unit:
        text(draw, (x + 78, 377), unit, 10, "#909399")

section(425, "年龄-SOS参考与历史测量")
text(draw, (right, 442), "横坐标为每次检测当时年龄", 11, "#909399", False, "ra")
draw.rounded_rectangle((left, 456, right, 822), radius=7, fill="white", outline="#d9dee5", width=1)

# Actual product chart background plus proposed history points.
chart_box = (67, 469, 550, 824)
chart = Image.open(CHART).convert("RGB").resize((chart_box[2] - chart_box[0], chart_box[3] - chart_box[1]))
canvas.paste(chart, chart_box[:2])
draw = ImageDraw.Draw(canvas)

def point(x_ratio, y_ratio, current=False):
    x = chart_box[0] + int((chart_box[2] - chart_box[0]) * x_ratio)
    y = chart_box[1] + int((chart_box[3] - chart_box[1]) * y_ratio)
    radius = 8 if current else 6
    color = "#f56c6c" if current else "#7f8c98"
    draw.ellipse((x - radius - 2, y - radius - 2, x + radius + 2, y + radius + 2), fill="white")
    draw.ellipse((x - radius, y - radius, x + radius, y + radius), fill=color)
    return x, y


point(.295, .571)
point(.326, .526)
px, py = point(.357, .481, True)
label = (px + 12, py - 30, px + 160, py - 4)
draw.rounded_rectangle(label, radius=4, fill="#fffafa", outline="#f3a4a4", width=1)
text(draw, (label[0] + 7, label[1] + 5), "本次：47岁  4090 m/s", 10, "#d94e4e", True)

# History list beside chart
text(draw, (566, 478), "历史记录", 14, "#606266", True)
history = [
    ("41岁", "3980 m/s", "2020-08-16", False),
    ("44岁", "4035 m/s", "2023-07-09", False),
    ("47岁", "4090 m/s", "2026-09-22  本次", True),
]
for i, (age, sos, date, current) in enumerate(history):
    y = 515 + i * 78
    text(draw, (566, y), age, 11, "#909399")
    text(draw, (610, y), sos, 13, "#e95d5d" if current else "#303133", True)
    text(draw, (610, y + 24), date, 9, "#909399")
    if i < 2:
        draw.line((566, y + 55, 714, y + 55), fill="#ebeef5", width=1)

section(842, "诊断提示")
draw.rounded_rectangle((left, 868, right, 941), radius=7, fill="white", outline="#d9dee5", width=1)
text(draw, (70, 882), "骨强度评估：正常；相对骨折风险：低；相对骨龄：45 岁。", 12, "#606266")
text(draw, (70, 907), "灰色点为该患者既往检测，红色点为本报表对应的检测结果。", 12, "#606266")
text(draw, (495, 964), "检查医师：", 13, "#606266")
draw.line((575, 982, 722, 982), fill="#606266", width=1)
draw.line((left, 1002, right, 1002), fill="#606266", width=1)
text(draw, (left, 1016), "只做临床参考，不作证明材料", 10, "#606266")
text(draw, (right, 1016), "联系电话：________________", 10, "#606266", False, "ra")

# Review notes
note = (786, 28, 1098, 866)
draw.rounded_rectangle(note, radius=11, fill="white")
text(draw, (808, 54), "本次样稿的显示规则", 20, "#20252b", True)
text(draw, (808, 88), "先确认结构和信息表达，正式实现继续\n使用项目现有年龄-SOS参考底图。", 12, "#76818c")

rules = [
    ("报表红点", "表示当前打开的这份报表对应的那次\n测量，不会误用患者今天的年龄。"),
    ("历史灰点", "同一患者以前的有效测量，按每条记录\n当时的年龄落点。"),
    ("上位机主页", "显示该患者全部有效历史点；最新一次\n为红色，其余为灰色。"),
    ("旧数据兼容", "优先使用保存的测量时年龄；缺失时用\n出生日期和测量日期计算，不改 XML。"),
]
for i, (heading, body) in enumerate(rules):
    y = 145 + i * 132
    draw.rounded_rectangle((806, y, 1078, y + 106), radius=6, fill="#f5f8fb")
    draw.rectangle((806, y, 810, y + 106), fill="#1296e8")
    text(draw, (824, y + 13), heading, 13, "#0b86d4", True)
    text(draw, (824, y + 42), body, 11, "#4f5963")

draw.ellipse((815, 697, 826, 708), fill="#f56c6c")
text(draw, (835, 694), "本次／最新", 11, "#606266")
draw.ellipse((925, 697, 936, 708), fill="#7f8c98")
text(draw, (945, 694), "历史测量", 11, "#606266")

draw.rounded_rectangle((806, 744, 1078, 838), radius=7, fill="#fff7e8", outline="#f2c875", width=1)
text(draw, (824, 760), "等待确认的重点", 13, "#a76b00", True)
text(draw, (824, 788), "① 报表是否显示全部历史点\n② 右侧历史列表是否保留\n③ 整体大小和位置是否合适", 11, "#6f5a31")

canvas.save(OUTPUT, quality=95)
print(OUTPUT)
