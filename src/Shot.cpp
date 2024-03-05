#include<screenshot/Shot.h>
#include<algorithm>
using std::max;
using std::min;
ShotIO shot_io;

void Screenshot::start_job(int x, int y, int width, int height, int shot_type){
    printf("start job %d\n", is_running);

    old_shot.release();
    while(1){
        if (!is_running){
            printf("stop job\n");
            break;
        }
        auto now_shot = getScreenshot(x, y,width, height);
        // cv::imshow("a", now_shot);
        // cv::waitKey(0);
        if (old_shot.empty())
            old_shot = now_shot;
        else
            old_shot = concat_pic(old_shot,now_shot,shot_type);
        // cv::imshow("b", old_shot);
        // cv::waitKey(0);
        tmp1 = now_shot.clone();
        tmp2 = old_shot.clone();
        emit send_pic(tmp1, tmp2);
        QThread::msleep(200);
    }
}

void Screenshot::set_is_running(bool b){
    pthread_mutex_lock(&mutex);
    is_running = b;
    pthread_mutex_unlock(&mutex);
}

Screenshot::Screenshot()
{
    zoom = getZoom();
    // printf("zoom:%f\n", zoom);
    m_width = GetSystemMetrics(SM_CXSCREEN) * zoom;
    m_height = GetSystemMetrics(SM_CYSCREEN) * zoom;
    m_screenshotData = new char[m_width * m_height * 4];
    memset(m_screenshotData, 0, m_width);

    // 获取屏幕 DC
    m_screenDC = GetDC(NULL);
    m_compatibleDC = CreateCompatibleDC(m_screenDC);

    // 创建位图
    m_hBitmap = CreateCompatibleBitmap(m_screenDC, m_width, m_height);
    SelectObject(m_compatibleDC, m_hBitmap);
}

/* 获取整个屏幕的截图 */
Mat Screenshot::getScreenshot()
{
    // 得到位图的数据
    BitBlt(m_compatibleDC, 0, 0, m_width, m_height, m_screenDC, 0, 0, SRCCOPY);

    GetBitmapBits(m_hBitmap, m_width * m_height * 4, m_screenshotData);
    BITMAPINFO  bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height;  // 顶部开始的正向高度
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;  // 每个像素32位
    bmi.bmiHeader.biCompression = BI_RGB;
    GetDIBits(m_compatibleDC, m_hBitmap, 0, (UINT)m_height, m_screenshotData, &bmi, DIB_RGB_COLORS);

    // 创建图像
    Mat screenshot(m_height, m_width, CV_8UC4, m_screenshotData);

    return screenshot;
}

/** @brief 获取指定范围的屏幕截图
 * @param x 图像左上角的 X 坐标
 * @param y 图像左上角的 Y 坐标
 * @param width 图像宽度
 * @param height 图像高度
 */
Mat Screenshot::getScreenshot(int x, int y, int width, int height)
{
    Mat screenshot = getScreenshot();
    return screenshot(cv::Rect(x*zoom, y*zoom, width*zoom, height*zoom));
}

/* 获取屏幕缩放值 */
double Screenshot::getZoom()
{
    // 获取窗口当前显示的监视器
    HWND hWnd = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // 获取监视器逻辑宽度
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(hMonitor, &monitorInfo);
    int cxLogical = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);

    // 获取监视器物理宽度
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;

    return cxPhysical * 1.0 / cxLogical;
}

// 计算图像的特征
inline std::pair<std::vector<cv::KeyPoint>, cv::Mat> compute_feature(Mat &pic){
    // static cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
    cv::Ptr<cv::SIFT> detector = cv::SIFT::create();
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    detector->detectAndCompute(pic, cv::noArray(), keypoints, descriptors);
    return {keypoints, descriptors};
}

// 进行特征匹配
std::vector<cv::DMatch> feature_match(Mat &featuresA, Mat &featuresB, float ratioThresh=0.75f){
    // 使用FLANN匹配器
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    // static cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<cv::DMatch>> knnMatches;
    matcher->knnMatch(featuresA, featuresB, knnMatches, 2);

    // 应用比率测试
    std::vector<cv::DMatch> goodMatches;
    for (size_t i = 0; i < knnMatches.size(); i++) {
        if (knnMatches[i][0].distance < ratioThresh * knnMatches[i][1].distance) {
            goodMatches.push_back(knnMatches[i][0]);
        }
    }
    return goodMatches;
}

// 透视变换
bool getHomography(
    const std::vector<cv::KeyPoint>& kpsA, 
    const std::vector<cv::KeyPoint>& kpsB, 
    const std::vector<cv::DMatch>& matches, 
    cv::Mat &H,
    double reprojThresh) {

    if (matches.size() > 4) {
        // 构造点集
        std::vector<cv::Point2f> ptsA;
        std::vector<cv::Point2f> ptsB;
        for (const auto& match : matches) {
            ptsA.push_back(kpsA[match.queryIdx].pt);
            ptsB.push_back(kpsB[match.trainIdx].pt);
        }
        // 估计两组点之间的单应性矩阵
        std::vector<uchar> status;
        H = cv::findHomography(ptsA, ptsB, cv::RANSAC, reprojThresh, status);
        return true;
    } else {
        return false;
    }
}

// 去除黑边部分
Mat cut_black(Mat &a){
    int x1,x2,y1,y2;
    // 从上往下，左往右遍历
    for(int i = 0; i < a.rows; ++i){
        for(int j = 0; j < a.cols; ++j){
            cv::Vec3b color = a.at<cv::Vec3b>(i, j);
            // 非全黑像素
            if (color[0] != 0 || color[1] != 0 || color[2] != 0){
                y1 = i; // 这个是确定的
                goto end_top;
            }
        }
    }
    end_top:
    // 从下往上，左往右遍历
    for(int i = a.rows - 1; i >= 0; --i){
        for(int j = 0; j < a.cols; ++j){
            cv::Vec3b color = a.at<cv::Vec3b>(i, j);
            // 非全黑像素
            if (color[0] != 0 || color[1] != 0 || color[2] != 0){
                y2 = i; // 这个是确定的
                goto end_bottom;
            }
        }
    }
    end_bottom:
    // 遍历左边
    for(int j = 0; j <= a.cols; ++j){
        for(int i = y1; i <= y2; ++i){
            cv::Vec3b color = a.at<cv::Vec3b>(i, j);
            if (color[0] != 0 || color[1] != 0 || color[2] != 0){
                x1 = j;
                goto end_left;
            }
        }
    }
    end_left:
    // 遍历右边
    for(int j = a.cols - 1; j >= 0; --j){
        for(int i = y1; i <= y2; ++i){
            cv::Vec3b color = a.at<cv::Vec3b>(i, j);
            if (color[0] != 0 || color[1] != 0 || color[2] != 0){
                x2 = j;
                goto end_right;
            }
        }
    }
    end_right:
    return a(cv::Rect(x1, y1, x2 - x1, y2 - y1));
}

struct PointCompare {
    bool operator()(const cv::Point2f& a, const cv::Point2f& b) const {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

//拼接两张图
Mat Screenshot::concat_pic(Mat &b, Mat &a, int shot_type,bool is_show){
    //显示两张图
    if (is_show){
        cv::imshow("Original image (a)", a);
        cv::waitKey(0);
        cv::imshow("Original image (b)", b);
        cv::waitKey(0);
    }


    auto res_1 = compute_feature(a);
    auto res_2 = compute_feature(b);
    auto &kpsA = res_1.first;
    auto &kpsB = res_2.first;
    auto &featuresA = res_1.second;
    auto &featuresB = res_2.second;
    if (is_show){
        // 显示两张图的关键点
        cv::Mat outputImage;
        cv::drawKeypoints(a, kpsA, outputImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imshow("Image with SIFT KeyPoints", outputImage);
        cv::waitKey(0);

        cv::drawKeypoints(b, kpsB, outputImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imshow("Image with SIFT KeyPoints", outputImage);
        cv::waitKey(0);
    }

    // 特征匹配
    auto goodMatches = feature_match(featuresA, featuresB);
    // printf("match number:%d\n", goodMatches.size());
    if (is_show){
        //绘制匹配的结果
        cv::Mat imgMatches;
        cv::drawMatches(a, kpsA, b, kpsB, goodMatches, imgMatches, 
                        cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), 
                        cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        cv::imshow("Good Matches", imgMatches);
        cv::waitKey(0);
    }

    // 将匹配结果进行运算
    std::map<cv::Point2f, int, PointCompare> displacementFrequency;

    for (size_t i = 0; i < goodMatches.size(); i++) {
        cv::Point2f pointA = kpsA[goodMatches[i].queryIdx].pt;
        cv::Point2f pointB = kpsB[goodMatches[i].trainIdx].pt;
        cv::Point2f displacement = pointB-pointA;

        // 四舍五入位移以减少浮点数误差的影响
        displacement.x = round(displacement.x);
        displacement.y = round(displacement.y);

        displacementFrequency[displacement]++;
    }

    // 找到出现次数最多的位移向量
    int maxFrequency = 0;
    cv::Point2f mostCommonDisplacement;
    for (const auto& entry : displacementFrequency) {
        if (entry.second > maxFrequency) {
            mostCommonDisplacement = entry.first;
            maxFrequency = entry.second;
        }
    }

    // std::cout << "Most Common Displacement: " << mostCommonDisplacement <<maxFrequency << std::endl;
    // a图片相对于b图片的位移
    int dx = mostCommonDisplacement.x;
    int dy = mostCommonDisplacement.y;

    // 拼接两张图
    if (dy < 0){
        // 相对位移在上面，换位置
        cv::swap(a, b);
        dx = -dx;
        dy = -dy;
    }
    // 计算后来图片的长和宽
    int res_width, res_height;
    if (dx >= 0)
        res_width = max(dx + a.cols, b.cols);
    else
        res_width = max(-dx + b.cols, a.cols);
    res_height = max(dy + a.rows, b.rows);

    cv::Mat outputImage = cv::Mat::zeros(res_height, res_width, a.type());
    if (shot_type == 0){
        a.copyTo(outputImage(cv::Rect(max(0, dx), dy, a.cols, a.rows)));
        b.copyTo(outputImage(cv::Rect(max(0, -dx), 0, b.cols, b.rows)));
    } else if (shot_type == 1){
        b.copyTo(outputImage(cv::Rect(max(0, -dx), 0, b.cols, b.rows)));
        a.copyTo(outputImage(cv::Rect(max(0, dx), dy, a.cols, a.rows)));
    }
    return outputImage;
}

QImage cvMat2QImage(const cv::Mat &mat)
{
    QImage image;
    switch(mat.type())
    {
    case CV_8UC1:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped(); // BRG转为RGB
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case CV_8UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        image = image.rgbSwapped(); // BRG转为RGB
        break;
    }
    return image;
}