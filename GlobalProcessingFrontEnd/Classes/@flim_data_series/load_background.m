function load_background(obj, background_file)

    %> Load a background image from a file

    if isdir(background_file)
    % load a series of images
    
        files = dir([background_file filesep '*.tif']);
        
        im = 0;
        for i=1:length(files)
            imi = imread([background_file filesep files(i).name]);
            im = im + double(imi);
        end
        im = im / length(files);
        
    else
  
        im = imread(background_file);      
        im = double(im);
    
    end    
    
    
    % correct for labview broken tiffs
    if all(im > 2^15)
        im = im - 2^15;
    end
    
    
    
    %{
    extent = 3;
    kernel1 = ones([extent 1]) / extent;
    kernel2 = ones([1 extent]) / extent;
    
    filtered = conv2nan(im,kernel1);                
    im = conv2nan(filtered,kernel2); 
    %}
    
    extent = 3;
    im = medfilt2(im,[extent extent], 'symmetric');
    
    
    if any(size(im) ~= [obj.height obj.width])
        throw(MException('GlobalAnalysis:BackgroundIncorrectShape','Error loading background, file has different dimensions to the data'));
    else
        obj.background_image = im;
        obj.background_type = 2;
    end    

    obj.compute_tr_data();
    
end