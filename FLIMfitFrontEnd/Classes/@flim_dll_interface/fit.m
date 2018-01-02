 function err = fit(obj, data_series, fit_params, roi_mask, selected)

    % Copyright (C) 2013 Imperial College London.
    % All rights reserved.
    %
    % This program is free software; you can redistribute it and/or modify
    % it under the terms of the GNU General Public License as published by
    % the Free Software Foundation; either version 2 of the License, or
    % (at your option) any later version.
    %
    % This program is distributed in the hope that it will be useful,
    % but WITHOUT ANY WARRANTY; without even the implied warranty of
    % MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    % GNU General Public License for more details.
    %
    % You should have received a copy of the GNU General Public License along
    % with this program; if not, write to the Free Software Foundation, Inc.,
    % 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    %
    % This software tool was developed with support from the UK 
    % Engineering and Physical Sciences Council 
    % through  a studentship from the Institute of Chemical Biology 
    % and The Wellcome Trust through a grant entitled 
    % "The Open Microscopy Environment: Image Informatics for Biological Sciences" (Ref: 095931).

    % Author : Sean Warren


    obj.load_global_library();
   
    
    if nargin < 4
        roi_mask = [];
    end
    if nargin < 5
        selected = [];
    end
    
    % Check if a binning mask has been provided
    if nargin >= 5
        obj.bin = true;
    else
        obj.bin = false;
    end
    
    err = 0;
   
    % If called without arguments we're continuing a fit
    if nargin > 1
        
        obj.clear_fit();
        
        obj.data_series = data_series;
        obj.fit_params = fit_params;
        obj.fit_round = 1;
        
        obj.fit_in_progress = true;
        
        delete(obj.fit_result);
        obj.fit_result = flim_fit_result();    
        
        obj.fit_result.width = data_series.width;
        obj.fit_result.height = data_series.height;
        obj.fit_result.binned = obj.bin;
                
    end

    prof = get_profile();
    
    p = obj.fit_params;
    d = obj.data_series;
    
    obj.use_image_irf = d.has_image_irf && ~obj.bin && p.image_irf_mode == 1;
    
    
    % Determine which datasets we need to load and make sure they're loaded
    if p.global_scope < 2 || p.global_variable == 0
        if false && d.lazy_loading && p.split_fit
            obj.n_rounds = ceil(d.n_datasets/p.n_thread);

            idx = 1:d.n_datasets;
            
            idx = idx/p.n_thread;
            idx = ceil(idx);
            datasets = (idx==obj.fit_round) & d.use';
        else
            datasets = d.use';
            obj.n_rounds = 1;
        end
    else
        var = fieldnames(d.metadata);
        var = var{p.global_variable};
        
        vals = d.metadata.(var);
        
        var_is_numeric = all(cellfun(@isnumeric,vals));
        
        if var_is_numeric
            vals = cell2mat(vals);
            vals = unique(vals);
            vals = num2cell(vals);
        else
            vals = unique(vals);
        end
        
        obj.n_rounds = length(vals);
        cur_var = vals{obj.fit_round};
        
        if var_is_numeric
            datasets = cellfun(@(x) eq(x,cur_var),d.metadata.(var));
        else
            datasets = cellfun(@(x) strcmp(x,cur_var),d.metadata.(var));
        end
        
        datasets = datasets & d.use';
            
    end
     
    sel = 1:d.n_datasets;
    
    if obj.bin
        datasets = (sel == selected);
        obj.n_rounds = 1;
    end

    sel = sel(datasets);
    
    if d.lazy_loading
        d.load_selected_files(sel);
    end    

    if obj.bin
        obj.datasets = 1;
        use = 1;
        sel = 1;
    else
        obj.datasets = sel;
        use = datasets(d.loaded);
    end
    
    obj.use = use;
        
    obj.im_size = [d.height d.width];
   
    acq = struct();
    acq.data_type = strcmp(d.mode,'tcspc');
    acq.t_rep = 1e6/d.rep_rate;
    acq.polarisation_resolved = d.polarisation_resolved;
    acq.n_chan = d.n_chan;
    acq.counts_per_photon = d.counts_per_photon;
    
    if obj.bin
        acq.n_x = 1;
        acq.n_y = 1;
        acq.t = d.tr_t;
        acq.t_int = d.tr_t_int;
    
        data = single(d.get_roi(roi_mask,selected));
        
%        data = single(d.cur_tr_data(:,:,roi_mask));
        data = mean(data,3);
        im(1) = ff_FLIMImage('acquisition_parmeters',acq,'data',data); 
        
       
    else
        acq.n_x = d.width;
        acq.n_y = d.height;
        acq.t = d.t;
        acq.t_int = d.t_int;
    
        % todo: get this from data
        if d.use_memory_mapping
            
            switch d.data_type
                case 'single'
                    data_size = 4;
                case 'uint16'
                    data_size = 2;
            end
            
            
            offset_step = data_size * d.n_t * d.n_chan * d.height * d.width;

            for i=1:length(sel)
               offset = (sel(i)-1) * offset_step + d.mapfile_offset;
               im(i) = ff_FLIMImage('acquisition_parmeters',acq,'mapped_file',d.mapfile_name,'data_offset',offset,'data_class',d.data_type); 
            end
        else
            for i=1:length(sel)
               im(i) = ff_FLIMImage('acquisition_parmeters',acq,'data',d.data_series_mem(:,:,:,:,sel(i))); 
            end
        end

        if ~isempty(d.acceptor)
           for i=1:length(sel) 
               ff_FLIMImage(im(i),'SetAcceptor',d.acceptor(:,:,sel(i)));
           end
        end
        if ~isempty(d.seg_mask) || ~isempty(d.multid_mask)
           for i=1:length(sel)
               if isempty(d.seg_mask)
                   mask = ones([d.height d.width],'uint16');
               else
                   mask = d.seg_mask(:,:,sel(i));
               end
               if ~isempty(d.multid_mask)
                   mask(~d.multid_mask(:,:,sel(i))) = 0;
               end
               ff_FLIMImage(im(i),'SetMask',mask);
           end
        end
    end
    
    transform = struct();
    transform.smoothing_factor = d.binning;
    transform.t_start = d.t_min;
    transform.t_stop = d.t_max;
    transform.threshold = d.thresh_min;
    transform.limit = d.gate_max;
    
    irf = struct();
    irf.irf = d.tr_irf;
    irf.timebin_t0 = d.tr_t_irf(1);
    irf.timebin_width = d.tr_t_irf(2) - d.tr_t_irf(1);
    irf.ref_reconvolution = d.irf_type;
    irf.ref_lifetime_guess = d.ref_lifetime;
            
    if all(size(d.tvb_profile) == d.data_size(1:2)')
        tvb = d.tvb_profile;
    else
        tvb = zeros(d.data_size(1:2)');
    end
    
    data = ff_FLIMData('images',im,...
                       'data_transformation_settings',transform,...
                       'irf',irf,...
                       'background_value',d.background_value); %,...
                       %'tvb_profile',tvb,...
                       %'tvb_I_map',d.tvb_I_image);
       
    ff_FLIMImage(im,'Release');
                   
    % todo: background image
    
    
    fit_settings = struct();
    fit_settings.n_thread = p.n_thread;
    fit_settings.calculate_errors = p.calculate_errs;
    fit_settings.weighting = p.weighting_mode;
    fit_settings.global_algorithm = 1;
    fit_settings.global_scope = p.global_scope;
    fit_settings.algorithm = p.fitting_algorithm;
    % TODO ...
    
    fitting_options.max_iterations = prof.Fitting.Maximum_Iterations;
    fitting_options.initial_step_size = prof.Fitting.Initial_Step_Size;
    fitting_options.use_numerical_derivatives = p.use_numerical_derivatives;
     
    obj.dll_id = ff_Controller();
    ff_Controller(obj.dll_id,'ClearFit');
    ff_Controller(obj.dll_id,'SetData',data);
    ff_Controller(obj.dll_id,'SetModel',p.model);
    ff_Controller(obj.dll_id,'SetFitSettings',fit_settings);
    ff_Controller(obj.dll_id,'SetFittingOptions',fitting_options);
    
    ff_FLIMData(data,'Release')
    
    obj.start_time = tic;
   
    ff_Controller(obj.dll_id,'StartFit');
    
    if err ~= 0
        return;
    end
                   
    obj.fit_round = obj.fit_round + 1;

    if obj.fit_round == 2

        if err == 0
            obj.fit_timer = timer('TimerFcn',@obj.update_progress, 'ExecutionMode', 'fixedSpacing', 'Period', 0.1);
            start(obj.fit_timer)
        else
            obj.clear_temp_vars();
            msgbox(['Err = ' num2str(err)]);
        end
        
    end
    

end