#pragma once

#include <atlstr.h>
#include <iostream>
#include <cstring>
#include "image.h"
#include "goruntu_isleme.h"
#include "matris.h"
#include "ImageViewerForm.h"

namespace read_image {

    using ::Matris;
    using ::image;
    using ::point2D;

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:
        Form1(void)
        {
            InitializeComponent();
            current_rgb = nullptr;
        }

    protected:
        ~Form1()
        {
            if (current_rgb != nullptr)
            {
                free_matris(*current_rgb);
                delete current_rgb;
                current_rgb = nullptr;
            }
            if (components)
            {
                delete components;
            }
        }

    private: System::Windows::Forms::MenuStrip^ menuStrip1;
    private: System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
    private: System::Windows::Forms::ToolStripMenuItem^ openToolStripMenuItem;
    private: System::Windows::Forms::ToolStripMenuItem^ detectLinesToolStripMenuItem;
    private: System::Windows::Forms::ToolStripMenuItem^ detectCirclesToolStripMenuItem;
    private: System::Windows::Forms::PictureBox^ pictureBox1;
    private: System::Windows::Forms::OpenFileDialog^ openFileDialog1;
    private: System::ComponentModel::Container^ components;
    private: System::Windows::Forms::PictureBox^ pictureBoxHistogram;

           // Mevcut parametre kontrolleri
    private: System::Windows::Forms::GroupBox^ groupBoxParams;
    private: System::Windows::Forms::NumericUpDown^ numericLowThresh;
    private: System::Windows::Forms::NumericUpDown^ numericHighThresh;
    private: System::Windows::Forms::NumericUpDown^ numericHoughTheta;
    private: System::Windows::Forms::NumericUpDown^ numericHoughThreshold;
    private: System::Windows::Forms::NumericUpDown^ numericHoughMinRadius;
    private: System::Windows::Forms::NumericUpDown^ numericHoughMaxRadius;
    private: System::Windows::Forms::Button^ btnUpdateHistogram;

           // Parametre başlıkları (etiketler)
    private: System::Windows::Forms::Label^ labelLowThresh;
    private: System::Windows::Forms::Label^ labelHighThresh;
    private: System::Windows::Forms::Label^ labelThetaSteps;
    private: System::Windows::Forms::Label^ labelHoughThreshold;
    private: System::Windows::Forms::Label^ labelMinRadius;
    private: System::Windows::Forms::Label^ labelMaxRadius;

           // YENİ: İstenen adet için kontrol
    private: System::Windows::Forms::Label^ labelDesiredCount;
    private: System::Windows::Forms::NumericUpDown^ numericDesiredCount;

    private:
        Matris* current_rgb;

        void ShowMatrisInNewWindow(Matris m, String^ title) {
            image im = matris_to_image(m);
            Bitmap^ bmp = gcnew Bitmap(im.w, im.h);
            for (int y = 0; y < im.h; y++) {
                for (int x = 0; x < im.w; x++) {
                    int idx = (y * im.w + x) * im.c;
                    Color c;
                    if (im.c == 1) {
                        byte val = im.data[idx];
                        c = Color::FromArgb(val, val, val);
                    }
                    else {
                        c = Color::FromArgb(im.data[idx], im.data[idx + 1], im.data[idx + 2]);
                    }
                    bmp->SetPixel(x, y, c);
                }
            }
            free_image(im);
            ImageViewerForm^ form = gcnew ImageViewerForm(title, bmp);
            form->Show();
        }

        void ShowImage(Matris m) {
            image im = matris_to_image(m);
            pictureBox1->Width = im.w;
            pictureBox1->Height = im.h;
            pictureBox1->Refresh();
            Bitmap^ surface = gcnew Bitmap(im.w, im.h);
            pictureBox1->Image = surface;
            for (int y = 0; y < im.h; y++) {
                for (int x = 0; x < im.w; x++) {
                    int idx = (y * im.w + x) * im.c;
                    Color c = Color::FromArgb(im.data[idx], im.data[idx + 1], im.data[idx + 2]);
                    surface->SetPixel(x, y, c);
                }
            }
            free_image(im);
        }

        void InitializeComponent(void)
        {
            this->pictureBoxHistogram = (gcnew System::Windows::Forms::PictureBox());
            this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
            this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->detectLinesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->detectCirclesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());

            // Kontroller
            this->groupBoxParams = (gcnew System::Windows::Forms::GroupBox());
            this->numericLowThresh = (gcnew System::Windows::Forms::NumericUpDown());
            this->numericHighThresh = (gcnew System::Windows::Forms::NumericUpDown());
            this->numericHoughTheta = (gcnew System::Windows::Forms::NumericUpDown());
            this->numericHoughThreshold = (gcnew System::Windows::Forms::NumericUpDown());
            this->numericHoughMinRadius = (gcnew System::Windows::Forms::NumericUpDown());
            this->numericHoughMaxRadius = (gcnew System::Windows::Forms::NumericUpDown());
            this->btnUpdateHistogram = (gcnew System::Windows::Forms::Button());

            // Etiketler
            this->labelLowThresh = (gcnew System::Windows::Forms::Label());
            this->labelHighThresh = (gcnew System::Windows::Forms::Label());
            this->labelThetaSteps = (gcnew System::Windows::Forms::Label());
            this->labelHoughThreshold = (gcnew System::Windows::Forms::Label());
            this->labelMinRadius = (gcnew System::Windows::Forms::Label());
            this->labelMaxRadius = (gcnew System::Windows::Forms::Label());

            // YENİ: istenen adet
            this->labelDesiredCount = (gcnew System::Windows::Forms::Label());
            this->numericDesiredCount = (gcnew System::Windows::Forms::NumericUpDown());

            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxHistogram))->BeginInit();
            this->menuStrip1->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
            this->groupBoxParams->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericLowThresh))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHighThresh))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughTheta))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughThreshold))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughMinRadius))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughMaxRadius))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericDesiredCount))->BeginInit();
            this->SuspendLayout();

            // 
            // pictureBoxHistogram
            // 
            this->pictureBoxHistogram->BackColor = System::Drawing::Color::White;
            this->pictureBoxHistogram->Location = System::Drawing::Point(650, 50);
            this->pictureBoxHistogram->Name = L"pictureBoxHistogram";
            this->pictureBoxHistogram->Size = System::Drawing::Size(350, 300);
            this->pictureBoxHistogram->TabIndex = 0;
            this->pictureBoxHistogram->TabStop = false;

            // 
            // menuStrip1
            // 
            this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^ >(1) { this->fileToolStripMenuItem });
            this->menuStrip1->Location = System::Drawing::Point(0, 0);
            this->menuStrip1->Name = L"menuStrip1";
            this->menuStrip1->Size = System::Drawing::Size(1033, 24);
            this->menuStrip1->TabIndex = 0;
            this->menuStrip1->Text = L"menuStrip1";

            // 
            // fileToolStripMenuItem
            // 
            this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^ >(3) {
                this->openToolStripMenuItem,
                    this->detectLinesToolStripMenuItem, this->detectCirclesToolStripMenuItem
            });
            this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
            this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
            this->fileToolStripMenuItem->Text = L"File";

            // 
            // openToolStripMenuItem
            // 
            this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
            this->openToolStripMenuItem->Size = System::Drawing::Size(146, 22);
            this->openToolStripMenuItem->Text = L"Open";
            this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openToolStripMenuItem_Click);

            // 
            // detectLinesToolStripMenuItem
            // 
            this->detectLinesToolStripMenuItem->Name = L"detectLinesToolStripMenuItem";
            this->detectLinesToolStripMenuItem->Size = System::Drawing::Size(146, 22);
            this->detectLinesToolStripMenuItem->Text = L"Detect Lines";
            this->detectLinesToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::detectLinesToolStripMenuItem_Click);

            // 
            // detectCirclesToolStripMenuItem
            // 
            this->detectCirclesToolStripMenuItem->Name = L"detectCirclesToolStripMenuItem";
            this->detectCirclesToolStripMenuItem->Size = System::Drawing::Size(146, 22);
            this->detectCirclesToolStripMenuItem->Text = L"Detect Circles";
            this->detectCirclesToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::detectCirclesToolStripMenuItem_Click);

            // 
            // pictureBox1
            // 
            this->pictureBox1->Location = System::Drawing::Point(13, 29);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(608, 495);
            this->pictureBox1->TabIndex = 1;
            this->pictureBox1->TabStop = false;

            // 
            // openFileDialog1
            // 
            this->openFileDialog1->FileName = L"openFileDialog1";

            // 
            // groupBoxParams
            // 
            this->groupBoxParams->Controls->Add(this->labelDesiredCount);
            this->groupBoxParams->Controls->Add(this->numericDesiredCount);
            this->groupBoxParams->Controls->Add(this->labelMaxRadius);
            this->groupBoxParams->Controls->Add(this->labelMinRadius);
            this->groupBoxParams->Controls->Add(this->labelHoughThreshold);
            this->groupBoxParams->Controls->Add(this->labelThetaSteps);
            this->groupBoxParams->Controls->Add(this->labelHighThresh);
            this->groupBoxParams->Controls->Add(this->labelLowThresh);
            this->groupBoxParams->Controls->Add(this->numericHoughMaxRadius);
            this->groupBoxParams->Controls->Add(this->numericHoughMinRadius);
            this->groupBoxParams->Controls->Add(this->numericHoughThreshold);
            this->groupBoxParams->Controls->Add(this->numericHoughTheta);
            this->groupBoxParams->Controls->Add(this->numericHighThresh);
            this->groupBoxParams->Controls->Add(this->numericLowThresh);
            this->groupBoxParams->Controls->Add(this->btnUpdateHistogram);
            this->groupBoxParams->Location = System::Drawing::Point(650, 360);
            this->groupBoxParams->Name = L"groupBoxParams";
            this->groupBoxParams->Size = System::Drawing::Size(350, 250);  // biraz daha yüksek
            this->groupBoxParams->TabIndex = 2;
            this->groupBoxParams->TabStop = false;
            this->groupBoxParams->Text = L"Parameters";

            // 
            // labelLowThresh
            // 
            this->labelLowThresh->AutoSize = true;
            this->labelLowThresh->Location = System::Drawing::Point(20, 10);
            this->labelLowThresh->Name = L"labelLowThresh";
            this->labelLowThresh->Size = System::Drawing::Size(72, 13);
            this->labelLowThresh->TabIndex = 7;
            this->labelLowThresh->Text = L"Low Thresh:";

            // 
            // labelHighThresh
            // 
            this->labelHighThresh->AutoSize = true;
            this->labelHighThresh->Location = System::Drawing::Point(200, 10);
            this->labelHighThresh->Name = L"labelHighThresh";
            this->labelHighThresh->Size = System::Drawing::Size(72, 13);
            this->labelHighThresh->TabIndex = 8;
            this->labelHighThresh->Text = L"High Thresh:";

            // 
            // labelThetaSteps
            // 
            this->labelThetaSteps->AutoSize = true;
            this->labelThetaSteps->Location = System::Drawing::Point(20, 50);
            this->labelThetaSteps->Name = L"labelThetaSteps";
            this->labelThetaSteps->Size = System::Drawing::Size(66, 13);
            this->labelThetaSteps->TabIndex = 9;
            this->labelThetaSteps->Text = L"Theta Steps:";

            // 
            // labelHoughThreshold
            // 
            this->labelHoughThreshold->AutoSize = true;
            this->labelHoughThreshold->Location = System::Drawing::Point(200, 50);
            this->labelHoughThreshold->Name = L"labelHoughThreshold";
            this->labelHoughThreshold->Size = System::Drawing::Size(85, 13);
            this->labelHoughThreshold->TabIndex = 10;
            this->labelHoughThreshold->Text = L"Hough Thresh:";

            // 
            // labelMinRadius
            // 
            this->labelMinRadius->AutoSize = true;
            this->labelMinRadius->Location = System::Drawing::Point(20, 90);
            this->labelMinRadius->Name = L"labelMinRadius";
            this->labelMinRadius->Size = System::Drawing::Size(60, 13);
            this->labelMinRadius->TabIndex = 11;
            this->labelMinRadius->Text = L"Min Radius:";

            // 
            // labelMaxRadius
            // 
            this->labelMaxRadius->AutoSize = true;
            this->labelMaxRadius->Location = System::Drawing::Point(200, 90);
            this->labelMaxRadius->Name = L"labelMaxRadius";
            this->labelMaxRadius->Size = System::Drawing::Size(64, 13);
            this->labelMaxRadius->TabIndex = 12;
            this->labelMaxRadius->Text = L"Max Radius:";

            // YENİ: istenen adet etiketi ve sayısal giriş
            this->labelDesiredCount->AutoSize = true;
            this->labelDesiredCount->Location = System::Drawing::Point(20, 130);
            this->labelDesiredCount->Name = L"labelDesiredCount";
            this->labelDesiredCount->Size = System::Drawing::Size(73, 13);
            this->labelDesiredCount->TabIndex = 13;
            this->labelDesiredCount->Text = L"Desired Count:";

            this->numericDesiredCount->Location = System::Drawing::Point(20, 146);
            this->numericDesiredCount->Minimum = System::Decimal(1);
            this->numericDesiredCount->Maximum = System::Decimal(1000);
            this->numericDesiredCount->Value = System::Decimal(15);
            this->numericDesiredCount->Name = L"numericDesiredCount";
            this->numericDesiredCount->Size = System::Drawing::Size(120, 20);
            this->numericDesiredCount->TabIndex = 14;

            // 
            // numericLowThresh
            // 
            this->numericLowThresh->DecimalPlaces = 2;
            this->numericLowThresh->Increment = System::Decimal(0.01);
            this->numericLowThresh->Location = System::Drawing::Point(20, 26);
            this->numericLowThresh->Maximum = System::Decimal(1.0);
            this->numericLowThresh->Minimum = System::Decimal(0.0);
            this->numericLowThresh->Name = L"numericLowThresh";
            this->numericLowThresh->Size = System::Drawing::Size(120, 20);
            this->numericLowThresh->TabIndex = 0;
            this->numericLowThresh->Value = System::Decimal(0.03);

            // 
            // numericHighThresh
            // 
            this->numericHighThresh->DecimalPlaces = 2;
            this->numericHighThresh->Increment = System::Decimal(0.01);
            this->numericHighThresh->Location = System::Drawing::Point(200, 26);
            this->numericHighThresh->Maximum = System::Decimal(1.0);
            this->numericHighThresh->Minimum = System::Decimal(0.0);
            this->numericHighThresh->Name = L"numericHighThresh";
            this->numericHighThresh->Size = System::Drawing::Size(120, 20);
            this->numericHighThresh->TabIndex = 1;
            this->numericHighThresh->Value = System::Decimal(0.08);

            // 
            // numericHoughTheta
            // 
            this->numericHoughTheta->Location = System::Drawing::Point(20, 66);
            this->numericHoughTheta->Maximum = System::Decimal(360);
            this->numericHoughTheta->Minimum = System::Decimal(1);
            this->numericHoughTheta->Name = L"numericHoughTheta";
            this->numericHoughTheta->Size = System::Drawing::Size(120, 20);
            this->numericHoughTheta->TabIndex = 2;
            this->numericHoughTheta->Value = System::Decimal(360);

            // 
            // numericHoughThreshold
            // 
            this->numericHoughThreshold->Location = System::Drawing::Point(200, 66);
            this->numericHoughThreshold->Maximum = System::Decimal(5000);
            this->numericHoughThreshold->Minimum = System::Decimal(0);
            this->numericHoughThreshold->Name = L"numericHoughThreshold";
            this->numericHoughThreshold->Size = System::Drawing::Size(120, 20);
            this->numericHoughThreshold->TabIndex = 3;
            this->numericHoughThreshold->Value = System::Decimal(150);

            // 
            // numericHoughMinRadius
            // 
            this->numericHoughMinRadius->Location = System::Drawing::Point(20, 106);
            this->numericHoughMinRadius->Maximum = System::Decimal(500);
            this->numericHoughMinRadius->Minimum = System::Decimal(1);
            this->numericHoughMinRadius->Name = L"numericHoughMinRadius";
            this->numericHoughMinRadius->Size = System::Drawing::Size(120, 20);
            this->numericHoughMinRadius->TabIndex = 4;
            this->numericHoughMinRadius->Value = System::Decimal(2);

            // 
            // numericHoughMaxRadius
            // 
            this->numericHoughMaxRadius->Location = System::Drawing::Point(200, 106);
            this->numericHoughMaxRadius->Maximum = System::Decimal(500);
            this->numericHoughMaxRadius->Minimum = System::Decimal(1);
            this->numericHoughMaxRadius->Name = L"numericHoughMaxRadius";
            this->numericHoughMaxRadius->Size = System::Drawing::Size(120, 20);
            this->numericHoughMaxRadius->TabIndex = 5;
            this->numericHoughMaxRadius->Value = System::Decimal(50);

            // 
            // btnUpdateHistogram
            // 
            this->btnUpdateHistogram->Location = System::Drawing::Point(20, 190);
            this->btnUpdateHistogram->Name = L"btnUpdateHistogram";
            this->btnUpdateHistogram->Size = System::Drawing::Size(120, 30);
            this->btnUpdateHistogram->TabIndex = 6;
            this->btnUpdateHistogram->Text = L"Update Histogram";
            this->btnUpdateHistogram->UseVisualStyleBackColor = true;
            this->btnUpdateHistogram->Click += gcnew System::EventHandler(this, &Form1::btnUpdateHistogram_Click);

            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1033, 750);  // yükseklik artırıldı
            this->Controls->Add(this->groupBoxParams);
            this->Controls->Add(this->pictureBoxHistogram);
            this->Controls->Add(this->pictureBox1);
            this->Controls->Add(this->menuStrip1);
            this->MainMenuStrip = this->menuStrip1;
            this->Name = L"Form1";
            this->Text = L"Form1";

            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxHistogram))->EndInit();
            this->menuStrip1->ResumeLayout(false);
            this->menuStrip1->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            this->groupBoxParams->ResumeLayout(false);
            this->groupBoxParams->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericLowThresh))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHighThresh))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughTheta))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughThreshold))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughMinRadius))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericHoughMaxRadius))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericDesiredCount))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();
        }

    private: System::Void openToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
        if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            CString str = openFileDialog1->FileName;
            CStringA s2(str);
            const char* input = s2;
            if (current_rgb != nullptr) {
                free_matris(*current_rgb);
                delete current_rgb;
                current_rgb = nullptr;
            }
            current_rgb = load_image(input);
            ShowImage(*current_rgb);
            std::cout << "w: " << current_rgb->w << ", h: " << current_rgb->h << ", c: " << current_rgb->c << std::endl;
        }
    }

    void DrawGradientHistogram() {
         if (current_rgb == nullptr) return;

         Matris gray = rgbToIntensity(*current_rgb);
         Matris smooth = Smoothing(gray);
         int totalPixels = smooth.w * smooth.h;
         float* angles = new float[totalPixels];
         Matris grad = Gradient(smooth, angles);

         float maxGrad = 0;
         for (int i = 0; i < totalPixels; i++) if (grad.data[i] > maxGrad) maxGrad = grad.data[i];
         if (maxGrad == 0) 
             maxGrad = 1;

         const int numBins = 1000;
         int* hist = new int[numBins]();
         for (int i = 0; i < totalPixels; i++) {
              int bin = (int)((grad.data[i] / maxGrad) * (numBins - 1));
              bin = max(0, min(numBins - 1, bin));
              hist[bin]++;
         }

         int maxFreq = 0;
         for (int i = 0; i < numBins; i++) if (hist[i] > maxFreq) maxFreq = hist[i];
         if (maxFreq == 0) maxFreq = 1;

         int w = pictureBoxHistogram->Width;
         int h = pictureBoxHistogram->Height;
         int marginLeft = 60, marginBottom = 50, marginTop = 20;
         int plotW = w - marginLeft - 20;
         int plotH = h - marginTop - marginBottom;

         Bitmap^ bmp = gcnew Bitmap(w, h);
         Graphics^ g = Graphics::FromImage(bmp);
         g->Clear(Color::White);
         g->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::AntiAlias;

         float binWidth = (float)plotW / numBins;
         Pen^ gridPen = gcnew Pen(Color::Gray, 1.0f);
         for (int i = 0; i <= 5; i++) {
              float y = marginTop + i * (plotH / 5.0f);
              g->DrawLine(gridPen, (float)marginLeft, y, (float)marginLeft + plotW, y);
              float x = marginLeft + i * (plotW / 5.0f);
              g->DrawLine(gridPen, x, (float)marginTop, x, (float)marginTop + plotH);
         }

         for (int i = 1; i < numBins; i++) {
             float x1 = marginLeft + (i - 1) * binWidth;
             float y1 = marginTop + plotH - ((float)hist[i - 1] / maxFreq) * plotH;
             float x2 = marginLeft + i * binWidth;
             float y2 = marginTop + plotH - ((float)hist[i] / maxFreq) * plotH;
             array<PointF>^ pts = gcnew array<PointF>(4);
             pts[0] = PointF(x1, marginTop + plotH);
             pts[1] = PointF(x1, y1);
             pts[2] = PointF(x2, y2);
             pts[3] = PointF(x2, marginTop + plotH);
             g->FillPolygon(Brushes::DarkBlue, pts);
         }

         g->DrawLine(Pens::Black, marginLeft, marginTop + plotH, marginLeft + plotW, marginTop + plotH);
         g->DrawLine(Pens::Black, marginLeft, marginTop, marginLeft, marginTop + plotH);

         int stepX = 499;
         int maxVal = ((numBins / stepX) + 1) * stepX;
         for (int val = 0; val <= maxVal; val += stepX) {
              float x = marginLeft + ((float)val / (numBins - 1)) * plotW;
              g->DrawLine(Pens::Black, x, (float)marginTop + plotH, x, (float)marginTop + plotH + 5);
              g->DrawString(val.ToString(), gcnew System::Drawing::Font("Arial", 8), Brushes::Black, x - 15, marginTop + plotH + 5);
         }

         for (int i = 0; i <= 5; i++) {
              float val = i * maxFreq / 5.0f;
              float y = marginTop + plotH - (val / maxFreq) * plotH;
              g->DrawLine(Pens::Black, (float)marginLeft - 5, y, (float)marginLeft, y);
              g->DrawString(((int)val).ToString(), gcnew System::Drawing::Font("Arial", 8), Brushes::Black, 5, y - 5);
         }

         g->DrawString("Gradient Histogram", gcnew System::Drawing::Font("Arial", 10), Brushes::Black, marginLeft + 80, 5);

         pictureBoxHistogram->Image = bmp;

         delete g;
         delete gridPen;
         delete[] hist;
         delete[] angles;
         free_matris(gray);
         free_matris(smooth);
         free_matris(grad);
    }

    private: System::Void btnUpdateHistogram_Click(System::Object^ sender, System::EventArgs^ e) {
        DrawGradientHistogram();
    }

    private: System::Void detectLinesToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
        if (current_rgb == nullptr) return;

        float lowThresh = (float)numericLowThresh->Value;
        float highThresh = (float)numericHighThresh->Value;
        int thetaSteps = (int)numericHoughTheta->Value;
        int houghThreshold = (int)numericHoughThreshold->Value;
        int desiredLines = (int)numericDesiredCount->Value;

        Matris gray = rgbToIntensity(*current_rgb);
        ShowMatrisInNewWindow(gray, "Intensity");

        Matris smooth = Smoothing(gray);
        ShowMatrisInNewWindow(smooth, "Smoothing");

        float* angles;
        Matris grad = Gradient(smooth, angles);
        ShowMatrisInNewWindow(grad, "Gradient Magnitude");

        Matris thin = MaxSuppression(grad, angles);
        ShowMatrisInNewWindow(thin, "Non-max Suppression");

        Matris edge = CannyEdge(thin, lowThresh, highThresh);
        ShowMatrisInNewWindow(edge, "Binary Edge");

        // Hough uzayını göster
        int rhoSteps = 500; // isteğe bağlı, yeterince büyük
        Matris houghSpace = GetHoughSpaceImage(edge, thetaSteps, rhoSteps);
        ShowMatrisInNewWindow(houghSpace, "Hough Space (Lines)");
        free_matris(houghSpace);

        int num_lines = 0;
        float* lines = Hough_Line(edge, angles, num_lines, thetaSteps, houghThreshold, desiredLines);
        Console::WriteLine("Number of lines detected: {0}", num_lines);

        Matris result_copy = make_matris(current_rgb->w, current_rgb->h, current_rgb->c);
        memcpy(result_copy.data, current_rgb->data, current_rgb->w * current_rgb->h * current_rgb->c * sizeof(float));
        DrawSelectedLines(result_copy, lines, num_lines);
        ShowImage(result_copy);
        free_matris(result_copy);

        free_matris(gray);
        free_matris(smooth);
        free_matris(grad);
        free_matris(thin);
        free_matris(edge);
        delete[] angles;
        delete[] lines;

        DrawGradientHistogram();
    }

   private: System::Void detectCirclesToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
       if (current_rgb == nullptr) return;

       float lowThresh = (float)numericLowThresh->Value;
       float highThresh = (float)numericHighThresh->Value;
       int minRadius = (int)numericHoughMinRadius->Value;
       int maxRadius = (int)numericHoughMaxRadius->Value;
       int houghThreshold = (int)numericHoughThreshold->Value;
       int desiredCircles = (int)numericDesiredCount->Value;

       Matris gray = rgbToIntensity(*current_rgb);
       ShowMatrisInNewWindow(gray, "Intensity");

       Matris smooth = Smoothing(gray);
       ShowMatrisInNewWindow(smooth, "Smoothing");

       float* angles;
       Matris grad = Gradient(smooth, angles);
       ShowMatrisInNewWindow(grad, "Gradient Magnitude");

       Matris thin = MaxSuppression(grad, angles);
       ShowMatrisInNewWindow(thin, "Non-max Suppression");

       Matris edge = CannyEdge(thin, lowThresh, highThresh);
       ShowMatrisInNewWindow(edge, "Canny Edge");

       int num_circles = 0;
       float* radii;
       point2D* centers = Hough_Circle(edge, angles, radii, num_circles, minRadius, maxRadius,
           houghThreshold, desiredCircles);
       Console::WriteLine("Number of circles detected: {0}", num_circles);

       Matris result_copy = make_matris(current_rgb->w, current_rgb->h, current_rgb->c);
       memcpy(result_copy.data, current_rgb->data, current_rgb->w * current_rgb->h * current_rgb->c * sizeof(float));
       DrawSelectedCircles(result_copy, centers, radii, num_circles);
       ShowImage(result_copy);
       free_matris(result_copy);

       free_matris(gray);
       free_matris(smooth);
       free_matris(grad);
       free_matris(thin);
       free_matris(edge);
       delete[] angles;
       delete[] centers;
       delete[] radii;

       DrawGradientHistogram();
   }
    };
}