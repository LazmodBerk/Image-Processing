#pragma once

namespace read_image {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class ImageViewerForm : public System::Windows::Forms::Form
    {
    public:
        ImageViewerForm(String^ title, Bitmap^ bmp)
        {
            InitializeComponent();
            this->Text = title;
            this->pictureBox1->Image = bmp;
            this->pictureBox1->SizeMode = PictureBoxSizeMode::AutoSize;
            this->AutoSize = true;
            this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
        }

    protected:
        ~ImageViewerForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::Windows::Forms::PictureBox^ pictureBox1;
        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
            this->SuspendLayout();
            // 
            // pictureBox1
            // 
            this->pictureBox1->Location = System::Drawing::Point(0, 0);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(247, 184);
            this->pictureBox1->TabIndex = 0;
            this->pictureBox1->TabStop = false;
            // 
            // ImageViewerForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(382, 264);
            this->Controls->Add(this->pictureBox1);
            this->Name = L"ImageViewerForm";
            this->Text = L"ImageViewerForm";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            this->ResumeLayout(false);

        }
    };
}