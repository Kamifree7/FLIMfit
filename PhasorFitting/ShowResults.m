function ShowResults(folder, end_filter)

    files = dir([folder '*' end_filter '.mat']);

    results = {};

    for i=1:length(files)

        load([folder files(i).name]);

        results{i} = r;
        names{i} = files(i).name;

    end

    sel = strfind(names, 'Post');
    sel = cellfun(@isempty,sel);

    results = results(sel);
    names = names(sel);

    names = strrep(names, '.pt3.mat', '');

    %%

    plot_results(5,0,1);
    plot_results(5,5,2);
    plot_results(10,10,3);
    
   
    function plot_results(q,off,idx)

        d = [];

        for j=1:q

            r = results{j+off};
            %{

            subplot(2,q,i);
            imagesc(r.E_CFP);
            PlotMerged(r.E_CFP, r.A_CFP, lim1)
            title(names{i+off}, 'Interpreter', 'none');

            subplot(2,q,i+q);
            imagesc(r.E_GFP);
            PlotMerged(r.E_GFP, r.A_GFP, lim2);
            title('GFP')
            %}
            sel = ~isnan(r.E_GFP);
            d = [d; [r.A_CFP(sel), r.A_GFP(sel), r.E_CFP(sel) r.E_GFP(sel)]];
        end


        col(1) = 3;
        col(2) = 4;

        x{1} = linspace(0,3,100);
        x{2} = linspace(0,3,100);
        x{3} = linspace(0,1,100);
        x{4} = linspace(0,1,100);

        label = {'A Rac', 'A Rho', 'E Rac', 'E Rho'};

        subplot(3,3,1+(idx-1)*3);

        [h,e] = hist3([d(:,col(1)) d(:,col(2))],[x(col(1)), x(col(2))]);
        imagesc(x{col(1)}, x{col(2)}, h)
        daspect([1 1 1])
        set(gca,'YDir','normal')
        xlabel(label{col(2)});
        ylabel(label{col(1)});

        for k=3:4
            subplot(3,3,k-1+(idx-1)*3);
            xx = x{k};
            [h,e] = histwv(d(:,k),d(:,k-2),min(xx),max(xx),length(xx));
            plot(xx,h);
            xlabel(label{k});
            ylim([0, 3000]);
        end
    end
end