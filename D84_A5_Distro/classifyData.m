%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CSC D84 - Assignment 6 - Classification using a random tree forest
%
% This function classifies a set of samples using a trained random
% tree forest. It outputs a confusion matrix, which is a 10x10
% matrix in which entry M(i,j) gives you the number of times that
% a sample whose correct class was 'i' was classified by the 
% random tree forest as being of class 'j'. 
%
% Which entries in this matrix should be large if the classifier is
% working?
%
% Note that you can run this both on the training and the testing
% datasets, but we only really care about results on test data.
%
% Starter code: F.J.E., March 18, 2014
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [sampleCM]=classifyData(samples,classes,forest)
% input samples ----------------------|       |       |
% patch classes so we can compute accuracy ---|       |
% Trained tree forest --------------------------------|

N=size(forest,1);	% Number of trees in this forest
sampleCM=zeros(10,10);	% Sample confusion matrix

fprintf(2,'Forest size=%d, Processing sample: ',N);
for i=1:size(samples,1);		% For all patches
	pch=samples(i,:);				% This is sample i. 
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   	% TO DO:
	%        Complete the classification function. Your code must
	%        test the current sample by passing it through each
	%        of the decision trees in the tree forest. Once a
	%	     terminal node is reached (leaf level node, or a node
	% 		 whose test is '-1') the leaf node 'votes' using its
	%		 class distribution. e.g. if the current leaf node's
	%	     class distribution is [1 0 2 5 0 2 3 1 9] then the
	%	     node adds 1 vote to class 0, 0 to class 1, 2 to class 2,
	%		 and so on.
	%
	%		 Once you have tested using all trees in the forest,
	%	     determine the most likely class for this sample, and
	%		 use the true class and your estimated class to update
	%		 the confusion matrix sampleCM
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	node_votes = zeros(1,10);

	for i = [1:N]
		index = 1;
		levels = log2((size(forest(i,:), 2) / 13 + 1) / 2);
		tree = reshape(forest(i,:),[(2^(levels+1))-1 13]);
		%disp(size(tree(index, 1)) / 2);
		while not (index > size(tree(index, 1)) / 2)
			pixel1 = tree(index, 1);
			pixel2 = tree(index, 2);
			test_id = forest(N, index, 3);
			res = test_pixels(pixel1, pixel2, test_id);
			if res == 1
				index = 2 * index;
			else res == 0
				index = (2 * index) + 1;
			end;
		end;
		node_votes = node_votes + tree(index, 4);
	end;
	[maximum, vote_index] = max(node_votes);
	sampleCM(classes(i), vote_index) = sampleCM(classes(i), vote_index) + 1;
end;

classCount=hist(classes,[1:10]);
fprintf(2,'Classifier accuracy per class:\n');
diag(sampleCM)./classCount'
fprintf(2,'Average correct classification rate.\n');
mean(diag(sampleCM)./classCount')
figure(2);clf;imagesc(sampleCM);axis image;colormap(jet);title('Confusion Matrix');

